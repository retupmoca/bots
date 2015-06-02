#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <math.h>

#include <bots/world.h>
#include <bots/cpu.h>

void bots_world_tick(bots_world* w){
    int i = 0;
    bots_shot* s = 0;
    /* physics */
    /* shots */
    for(s=w->shots[0], i=0; s; s=w->shots[++i]) {
        /* check collision */
        int hit = 0;
        int j;
        for(j=0; j < w->num_tanks; j++){
            if(   s->x >= w->tanks[j]->x - 40
               && s->x <= w->tanks[j]->x + 40
               && s->y >= w->tanks[j]->y - 40
               && s->y <= w->tanks[j]->y + 40
	       && w->tanks[j]->health > 0){
                /* hit! */
                hit = 1;

                /* record damage */
                w->tanks[j]->health -= 10;
                if(w->tanks[j]->health < 0)
                    w->tanks[j]->health = 0;

                /* delete the shot */
                free(s);
                int k;
                for(k=i; w->shots[k]; k++){
                    w->shots[k] = w->shots[k+1];
                }
                i--;
                break;
            }
        }
        if(hit)
            continue;

        /* move the shots */
        double rangle = (s->heading-64) * M_PI / 128;
        int dist = 20;
        int dx = floor(0.5 + (dist * cos(rangle)));
        int dy = floor(0.5 + (dist * sin(rangle)));
        s->x += dx;
        s->y += dy;

        /* check collision again */
        for(j=0; j < w->num_tanks; j++){
            if(   s->x >= w->tanks[j]->x - 40
               && s->x <= w->tanks[j]->x + 40
               && s->y >= w->tanks[j]->y - 40
               && s->y <= w->tanks[j]->y + 40
	       && w->tanks[j]->health > 0){
                /* hit! */

                /* record damage */
                w->tanks[j]->health -= 10;
                if(w->tanks[j]->health < 0)
                    w->tanks[j]->health = 0;

                /* delete the shot */
                free(s);
                int k;
                for(k=i; w->shots[k]; k++){
                    w->shots[k] = w->shots[k+1];
                }
                i--;
            }
        }
    }
    /* bots */
    for(i=0; i < w->num_tanks; i++){
        if(w->tanks[i]->health <= 0)
            continue;
        /* turn, etc */
        uint16_t throttle;
        short real_steering;
        uint16_t steering;
        uint16_t turret_steering;
        uint16_t scanner_steering;

        throttle = w->cpus[i]->ports[0] << 8;
        throttle = throttle | w->cpus[i]->ports[1];

        steering = w->cpus[i]->ports[2] << 8;
        steering = steering | w->cpus[i]->ports[3];
        steering = steering % 256;
        real_steering = steering;
        if(real_steering <= 128 && real_steering > 1){
            real_steering = 1;
        }
        if(real_steering > 128 && real_steering < 255){
            real_steering = 255;
        }
        steering -= real_steering;
        w->cpus[i]->ports[2] = steering >> 8;
        w->cpus[i]->ports[3] = steering & 0xff;

        w->tanks[i]->heading = (w->tanks[i]->heading + real_steering) % 256;
        w->tanks[i]->speed = throttle;
        if(w->tanks[i]->speed > 100)
            w->tanks[i]->speed = 100;

        /* drive! */
        double rangle = (w->tanks[i]->heading-64) * M_PI / 128;
        double dist = (w->tanks[i]->speed / 100.0) * 6.0;
        int dx = floor(0.5 + (dist * cos(rangle)));
        int dy = floor(0.5 + (dist * sin(rangle)));
        w->tanks[i]->x += dx;
        w->tanks[i]->y += dy;

        /* turn turret */
        turret_steering = w->cpus[i]->ports[4] << 8;
        turret_steering = turret_steering | w->cpus[i]->ports[5];
        if(w->cpus[i]->ports[7]) /* turret keepshift */
            turret_steering = (turret_steering + 256 - steering) % 256;
        real_steering = turret_steering;
        if(real_steering <= 128 && real_steering > 2){
            real_steering = 2;
        }
        if(real_steering > 128 && real_steering < 254){
            real_steering = 254;
        }
        turret_steering -= real_steering;
        w->cpus[i]->ports[4] = turret_steering >> 8;
        w->cpus[i]->ports[5] = turret_steering & 0xff;

        w->tanks[i]->turret_offset = (w->tanks[i]->turret_offset + real_steering) % 256;

        /* turn scanner */
        scanner_steering = w->cpus[i]->ports[8] << 8;
        scanner_steering = scanner_steering | w->cpus[i]->ports[9];

        if(w->cpus[i]->ports[0x0b]) /* scanner hull keepshift */
            scanner_steering = (scanner_steering + 256 - steering) % 256;

        if(w->cpus[i]->ports[0x0d]) /* scanner gun keepshift */
            scanner_steering = (scanner_steering + 256 - turret_steering) % 256;

        /* if we're keepshifting on the gun, but not the hull, we need to undo the
         * turret steering caused by its own keepshift
         */
        if(w->cpus[i]->ports[0x0d] && w->cpus[i]->ports[7] && !w->cpus[i]->ports[0x0b])
            scanner_steering = (scanner_steering + steering) % 256;

        w->tanks[i]->scanner_offset = (w->tanks[i]->scanner_offset + scanner_steering) % 256;
        w->cpus[i]->ports[8] = 0;
        w->cpus[i]->ports[9] = 0;
    }

    /* execute */
    for(i=0; i < w->num_tanks; i++){
        if(w->tanks[i]->health <= 0)
            continue;
        /* we do these "backwords" in order to simulate a 3-stage pipeline */
        bots_cpu_execute(w->cpus[i]);
        bots_cpu_decode(w->cpus[i]);
        bots_cpu_fetch(w->cpus[i]);
    }
}

void bots_world_add_bot(bots_world* w, bots_cpu* m, bots_tank* p) {
    if(w->num_tanks == 16){
        return;
    }
    m->world = w;
    m->bot_id = w->num_tanks;
    m->ports[0x17] = m->bot_id;
    w->cpus[w->num_tanks] = m;
    w->tanks[w->num_tanks] = p;
    w->num_tanks++;
}

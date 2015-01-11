#define _USE_MATH_DEFINES
#include <math.h>

#include "world.h"
#include "machine.h"

void world_tick(world* w){
    int i = 0;
    shot* s = NULL;
    /* physics */
    /* shots */
    for(s=w->shots[0], i=0; s; s=w->shots[++i]) {
        /* check collision */
        int hit = 0;
        for(int j=0; j < w->botcount; j++){
            if(   s->x >= w->botdata[j]->x - 5
               && s->x <= w->botdata[j]->x + 5
               && s->y >= w->botdata[j]->y - 5
               && s->y <= w->botdata[j]->y + 5){
                # hit!
                hit = 1;
                
                /* record damage */
                w->botdata[j]->health -= 5;
                if(w->botdata[j]->health < 0)
                    w->botdata[j]->health = 0;
                
                /* delete the shot */
                free(s);
                for(int k=i; w->shots[k]; k++){
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
        int dist = 5;
        int dx = floor(0.5 + (dist * cos(rangle)));
        int dy = floor(0.5 + (dist * sin(rangle)));
        s->x += dx;
        s->y += dy;
        
        /* check collision again */
        for(int j=0; j < w->botcount; j++){
            if(   s->x >= w->botdata[j]->x - 5
               && s->x <= w->botdata[j]->x + 5
               && s->y >= w->botdata[j]->y - 5
               && s->y <= w->botdata[j]->y + 5){
                # hit!
                
                /* record damage */
                w->botdata[j]->health -= 5;
                if(w->botdata[j]->health < 0)
                    w->botdata[j]->health = 0;
                
                /* delete the shot */
                free(s);
                for(int k=i; w->shots[k]; k++){
                    w->shots[k] = w->shots[k+1];
                }
                i--;
            }
        }
    }
    /* bots */
    for(i=0; i < w->botcount; i++){
        if(w->botdata[i]->health == 0)
            continue;
        /* turn, etc */
        uint16_t throttle;
        uint16_t steering;
        uint16_t turret_steering;
        uint16_t scanner_steering;
        
        throttle = w->bots[i]->ports[0] << 8;
        throttle = throttle | w->bots[i]->ports[1];
        
        steering = w->bots[i]->ports[2] << 8;
        steering = steering | w->bots[i]->ports[3];
        steering = steering % 256;
        
        w->botdata[i]->heading = (w->botdata[i]->heading + steering) % 256;
        w->bots[i]->ports[2] = 0;
        w->bots[i]->ports[3] = 0;
        w->botdata[i]->speed = throttle;

        /* drive! */
        double rangle = (w->botdata[i]->heading-64) * M_PI / 128;
        int dist = w->botdata[i]->speed;
        int dx = floor(0.5 + (dist * cos(rangle)));
        int dy = floor(0.5 + (dist * sin(rangle)));
        w->botdata[i]->x += dx;
        w->botdata[i]->y += dy;
        
        /* turn turret */
        turret_steering = w->bots[i]->ports[4] << 8;
        turret_steering = turret_steering | w->bots[i]->ports[5];
        if(w->bots[i]->ports[7]) /* turret keepshift */
            turret_steering = (turret_steering + 256 - steering) % 256;
        
        w->botdata[i]->turret_offset = (w->botdata[i]->turret_offset + turret_steering) % 256;
        w->bots[i]->ports[4] = 0;
        w->bots[i]->ports[5] = 0;
        
        /* turn scanner */
        scanner_steering = w->bots[i]->ports[8] << 8;
        scanner_steering = scanner_steering | w->bots[i]->ports[9];
        
        if(w->bots[i]->ports[0x0b]) /* scanner hull keepshift */
            scanner_steering = (scanner_steering + 256 - steering) % 256;
        
        if(w->bots[i]->ports[0x0d]) /* scanner gun keepshift */
            scanner_steering = (scanner_steering + 256 - turret_steering) % 256;
        
        /* if we're keepshifting on the gun, but not the hull, we need to undo the
         * turret steering caused by its own keepshift
         */
        if(w->bots[i]->ports[0x0d] && w->bots[i]->ports[7] && !w->bots[i]->ports[0x0b])
            scanner_steering = (scanner_steering + steering) % 256;
        
        w->botdata[i]->scanner_offset = (w->botdata[i]->scanner_offset + scanner_steering) % 256;
        w->bots[i]->ports[8] = 0;
        w->bots[i]->ports[9] = 0;
    }

    /* execute */
    for(i=0; i < w->botcount; i++){
        if(w->botdata[i]->health == 0)
            continue;
        /* we do these "backwords" in order to simulate a 3-stage pipeline */
        machine_execute(w->bots[i]);
        machine_decode(w->bots[i]);
        machine_fetch(w->bots[i]);
    }
}

void world_add_bot(world* w, machine* m, bot_physics* p) {
    if(w->botcount == 16){
        return;
    }
    m->world = w;
    m->machine_id = w->botcount;
    m->ports[0x17] = m->machine_id;
    w->bots[w->botcount] = m;
    w->botdata[w->botcount] = p;
    w->botcount++;
}

#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <math.h>

#include <bots/world.h>
#include <bots/cpu.h>

void _init_events(bots_world *w) {
    if(w->_tick_events == NULL) {
        w->_tick_events = malloc(sizeof(bots_events));
        w->_tick_events->events = malloc(sizeof(bots_event));
        w->_tick_events->_size = 1;
    }

    w->_tick_events->event_count = 0;
}

void _add_event(bots_world *w, uint8_t event_type, uint8_t bot_id) {
    if(w->_tick_events->event_count == w->_tick_events->_size) {
        w->_tick_events->_size *= 2;
        w->_tick_events->events = realloc(w->_tick_events->events,
                                          sizeof(bots_event)
                                           * w->_tick_events->_size);
    }
    bots_event *event = w->_tick_events->events
                        + (w->_tick_events->event_count)++;

    event->event_type = event_type;
    event->bot_id = bot_id;
}

void _physics_tick(bots_world *w) {
    /** run world physics **/
    int i = 0;
    bots_shot* s = 0;
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
                if(w->tanks[j]->health <= 0) {
                    w->tanks[j]->health = 0;
                    _add_event(w, BOTS_EVENT_DEATH, j);
                }

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
        double rangle = (s->heading) * M_PI / 128;
        int dist = 20;
        int dy = floor(0.5 + (dist * cos(rangle)));
        int dx = floor(0.5 + (dist * sin(rangle)));
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
                if(w->tanks[j]->health <= 0) {
                    w->tanks[j]->health = 0;
                    _add_event(w, BOTS_EVENT_DEATH, j);
                }

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
        short real_steering;
        short real_turret_steering;
        uint16_t steering;
        uint16_t turret_steering;
        uint16_t scanner_steering;
        uint16_t steering_adjust;

        steering = w->tanks[i]->_req_steering;

        real_steering = steering;
        if(real_steering <= 128 && real_steering > 1){
            real_steering = 1;
        }
        if(real_steering > 128 && real_steering < 255){
            real_steering = 255;
        }
        w->tanks[i]->_req_steering -= real_steering;

        w->tanks[i]->heading = (w->tanks[i]->heading + real_steering) % 256;
        w->tanks[i]->speed = w->tanks[i]->_req_throttle;
        if(w->tanks[i]->speed > 100)
            w->tanks[i]->speed = 100;

        /* drive! */
        double rangle = (w->tanks[i]->heading) * M_PI / 128;
        double dist = (w->tanks[i]->speed / 100.0) * 6.0;
        int dy = floor(0.5 + (dist * cos(rangle)));
        int dx = floor(0.5 + (dist * sin(rangle)));
        w->tanks[i]->x += dx;
        w->tanks[i]->y += dy;

        /* turn turret */
        turret_steering = w->tanks[i]->_req_turret_steering;
        if(w->tanks[i]->_req_turret_keepshift) /* turret keepshift */
            turret_steering = (turret_steering + 256 - real_steering) % 256;
        turret_steering = turret_steering % 256;

        real_turret_steering = turret_steering;
        if(real_turret_steering <= 128 && real_turret_steering > 2){
            real_turret_steering = 2;
        }
        if(real_turret_steering > 128 && real_turret_steering < 254){
            real_turret_steering = 254;
        }
        w->tanks[i]->_req_turret_steering -= real_turret_steering;

        w->tanks[i]->turret_offset = (w->tanks[i]->turret_offset + real_turret_steering) % 256;

        /* turn scanner */
        scanner_steering = w->tanks[i]->_req_scanner_steering;
        if(w->tanks[i]->_req_scanner_keepshift) /* scanner keepshift */
            scanner_steering = (scanner_steering + 256 - real_steering) % 256;

        w->tanks[i]->scanner_offset = (w->tanks[i]->scanner_offset + scanner_steering) % 256;
        w->tanks[i]->_req_scanner_steering = 0;
    }
}

void _process_tick(bots_world *w) {
    /** write I/O ports to CPU **/
    int i;
    for(i=0; i < w->num_tanks; i++){
        w->cpus[i]->ports[2] = w->tanks[i]->_req_steering >> 8;
        w->cpus[i]->ports[3] = w->tanks[i]->_req_steering & 0xff;

        w->cpus[i]->ports[4] = w->tanks[i]->_req_turret_steering >> 8;
        w->cpus[i]->ports[5] = w->tanks[i]->_req_turret_steering & 0xff;

        w->cpus[i]->ports[8] = w->tanks[i]->_req_scanner_steering >> 8;
        w->cpus[i]->ports[9] = w->tanks[i]->_req_scanner_steering & 0xff;

        w->cpus[i]->ports[0x18] = 0;
        w->cpus[i]->ports[0x19] = 0;

        w->cpus[i]->ports[0x1a] = 0;
        w->cpus[i]->ports[0x1b] = 0;
    }

    /** run CPU cycles **/
    for(i=0; i < w->num_tanks; i++){
        if(w->tanks[i]->health <= 0)
            continue;
        /* we do these "backwords" in order to simulate a 3-stage pipeline */
        bots_cpu_execute(w->cpus[i]);
        bots_cpu_decode(w->cpus[i]);
        bots_cpu_fetch(w->cpus[i]);

        bots_cpu_execute(w->cpus[i]);
        bots_cpu_decode(w->cpus[i]);
        bots_cpu_fetch(w->cpus[i]);
    }

    /** read I/O ports from CPU **/
    for(i=0; i < w->num_tanks; i++){
        int16_t throttle = w->cpus[i]->ports[0] << 8;
        throttle = throttle | w->cpus[i]->ports[1];
        w->tanks[i]->_req_throttle = throttle;

        uint16_t steering = w->cpus[i]->ports[2] << 8;
        steering = steering | w->cpus[i]->ports[3];
        uint16_t adjust_steering = w->cpus[i]->ports[0x18] << 8;
        adjust_steering |= w->cpus[i]->ports[0x19];
        steering += adjust_steering;
        steering = steering % 256;
        w->tanks[i]->_req_steering = steering;

        uint16_t turret_steering = w->cpus[i]->ports[4] << 8;
        turret_steering = turret_steering | w->cpus[i]->ports[5];
        uint16_t adjust_turret_steering = w->cpus[i]->ports[0x1a] << 8;
        adjust_turret_steering |= w->cpus[i]->ports[0x1b];
        turret_steering += adjust_turret_steering;
        turret_steering = turret_steering % 256;
        w->tanks[i]->_req_turret_steering = turret_steering;
        w->tanks[i]->_req_turret_keepshift = w->cpus[i]->ports[7];

        uint16_t scanner_steering = w->cpus[i]->ports[8] << 8;
        scanner_steering = scanner_steering | w->cpus[i]->ports[9];
        scanner_steering = scanner_steering % 256;
        w->tanks[i]->_req_scanner_steering = scanner_steering;
        w->tanks[i]->_req_scanner_keepshift = w->cpus[i]->ports[0x0b];
    }
}

bots_events* bots_world_tick(bots_world* w){
    _init_events(w);

    _physics_tick(w);
    _process_tick(w);

    return w->_tick_events;
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

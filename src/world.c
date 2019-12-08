#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <math.h>

#include <bots/world.h>
#include <bots/cpu.h>

static void _init_events(bots_world *w) {
    if(w->_tick_events == NULL) {
        w->_tick_events = malloc(sizeof(bots_events));
        w->_tick_events->events = malloc(sizeof(bots_event));
        w->_tick_events->_size = 1;
    }

    w->_tick_events->event_count = 0;
}

void bots_add_event(bots_world *w, uint8_t event_type, uint8_t bot_id) {
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
                bots_add_event(w, BOTS_EVENT_HIT, j);

                /* record damage */
                w->tanks[j]->health -= 10;
                if(w->tanks[j]->health <= 0) {
                    w->tanks[j]->health = 0;
                    bots_add_event(w, BOTS_EVENT_DEATH, j);
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
                bots_add_event(w, BOTS_EVENT_HIT, j);

                /* record damage */
                w->tanks[j]->health -= 10;
                if(w->tanks[j]->health <= 0) {
                    w->tanks[j]->health = 0;
                    bots_add_event(w, BOTS_EVENT_DEATH, j);
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
    int i;
    for(i=0; i < w->num_tanks; i++){
        if(w->tanks[i]->health <= 0)
            continue;

        /** write I/O ports to CPU **/
        for(int j=0; w->tanks[i]->peripherals[j].mem_base != 0; j++)
            w->tanks[i]->peripherals[j].process_tick(
                    w->tanks[i]->peripherals + j, w, i, 1);

        /** run CPU cycles **/
        for(int j=0; j<w->c_cpus_per_tick; j++)
            bots_cpu_cycle(w->cpus[i]);

        /** read I/O ports from CPU **/
        for(int j=0; w->tanks[i]->peripherals[j].mem_base != 0; j++)
            w->tanks[i]->peripherals[j].process_tick(
                    w->tanks[i]->peripherals + j, w, i, 0);
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
    m->bot_id = w->num_tanks;
    w->cpus[w->num_tanks] = m;
    w->tanks[w->num_tanks] = p;
    w->num_tanks++;
}

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
        int32_t start_x = s->x;
        int32_t start_y = s->y;
        s->x += dx;
        s->y += dy;
        if(abs(start_x - s->x) > dx * 2 || abs(start_y - s->y) > dy * 2) {
            /* we moved a long way...assume we wrapped around the edge */
            /* delete shot */
            free(s);
            int k;
            for(k=i; w->shots[k]; k++){
                w->shots[k] = w->shots[k+1];
            }
            i--;
            continue;
        }

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
        short real_scanner_steering;
        uint16_t steering;
        uint16_t turret_steering;
        uint16_t scanner_steering;
        uint16_t steering_adjust;

        steering = w->tanks[i]->_req_steering;

        real_steering = steering;
        if(real_steering <= 128 && real_steering > w->c_hull_turn_rate){
            real_steering = w->c_hull_turn_rate;
        }
        if(real_steering > 128 && real_steering < (256 - w->c_hull_turn_rate)){
            real_steering = 256 - w->c_hull_turn_rate;
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
        if(real_turret_steering <= 128 && real_turret_steering > w->c_turret_turn_rate){
            real_turret_steering = w->c_turret_turn_rate;
        }
        if(real_turret_steering > 128 && real_turret_steering < (256 - w->c_turret_turn_rate)){
            real_turret_steering = 256 - w->c_turret_turn_rate;
        }
        w->tanks[i]->_req_turret_steering -= real_turret_steering;

        w->tanks[i]->turret_offset = (w->tanks[i]->turret_offset + real_turret_steering) % 256;

        /* turn scanner */
        scanner_steering = w->tanks[i]->_req_scanner_steering;
        if(w->tanks[i]->_req_scanner_keepshift) /* scanner keepshift */
            scanner_steering = (scanner_steering + 256 - real_steering) % 256;
        scanner_steering = scanner_steering % 256;

        real_scanner_steering = scanner_steering;
        if(real_scanner_steering <= 128 && real_scanner_steering > w->c_scanner_turn_rate) {
            real_scanner_steering = w->c_scanner_turn_rate;
        }
        if(real_scanner_steering > 128 && real_scanner_steering < (256 - w->c_scanner_turn_rate)) {
            real_scanner_steering = 256 - w->c_scanner_turn_rate;
        }
        w->tanks[i]->_req_scanner_steering -= real_scanner_steering;

        w->tanks[i]->scanner_offset = (w->tanks[i]->scanner_offset + real_scanner_steering) % 256;
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
    if(w->num_tanks == BOTS_MAX_COUNT){
        return;
    }
    m->bot_id = w->num_tanks;
    w->cpus[w->num_tanks] = m;
    w->tanks[w->num_tanks] = p;
    w->num_tanks++;
}

void bots_world_place_bots(bots_world *w) {
    int32_t spawn_d = w->c_spawn_distance;

    double angle = 0;
    double step = (2 * M_PI) / w->num_tanks;

    for(int i=0; i<w->num_tanks; i++) {
        /* set bot to location */
        int32_t loc_x = floor(0.5 + (spawn_d * cos(angle)));
        int32_t loc_y = floor(0.5 + (spawn_d * sin(angle)));
        w->tanks[i]->x = loc_x;
        w->tanks[i]->y = loc_y;

        /* rotate location to next spawn location */
        angle += step;
    }
}

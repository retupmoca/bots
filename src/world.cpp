#include <bots/cpu.hpp>
#include <bots/world.hpp>

#include <cstdlib>
#include <cmath>

namespace bots {
    World::World(Options options = {}) : options(options) {
        _tick_events = (bots_events*)malloc(sizeof(bots_events));
        _tick_events->events = (bots_event*)malloc(sizeof(bots_event));
        _tick_events->_size = 1;

        _tick_events->event_count = 0;
    }

    World::~World() {
        int i;
        for(i = 0; i < num_tanks; i++) {
            free(tanks[i]);
            free(cpus[i]);
        }

        free(_tick_events->events);
        free(_tick_events);
    }

    void World::add_event(uint8_t event_type, uint8_t bot_id) {
        if(_tick_events->event_count == _tick_events->_size) {
            _tick_events->_size *= 2;
            _tick_events->events = (bots_event*)realloc(_tick_events->events,
                                           sizeof(bots_event)
                                            * _tick_events->_size);
        }
        bots_event *event = _tick_events->events
                            + (_tick_events->event_count)++;

        event->event_type = event_type;
        event->bot_id = bot_id;
    }

    void World::_physics_tick() {
        /** run world physics **/
        int i = 0;
        bots_shot* s = 0;
        /* shots */
        for(s=shots[0], i=0; s; s=shots[++i]) {
            /* check collision */
            int hit = 0;
            int j;
            for(j=0; j < num_tanks; j++){
                if(   s->x >= tanks[j]->x - 40
                   && s->x <= tanks[j]->x + 40
                   && s->y >= tanks[j]->y - 40
                   && s->y <= tanks[j]->y + 40
               && tanks[j]->health > 0){
                    /* hit! */
                    hit = 1;
                    add_event(BOTS_EVENT_HIT, j);

                    /* record damage */
                    tanks[j]->health -= 10;
                    if(tanks[j]->health <= 0) {
                        tanks[j]->health = 0;
                        add_event(BOTS_EVENT_DEATH, j);
                    }

                    /* delete the shot */
                    free(s);
                    int k;
                    for(k=i; shots[k]; k++){
                        shots[k] = shots[k+1];
                    }
                    i--;
                    break;
                }
            }
            if(hit)
                continue;

            /* move the shots */
            double rangle = (s->heading) * M_PI / 512;
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
                for(k=i; shots[k]; k++){
                    shots[k] = shots[k+1];
                }
                i--;
                continue;
            }

            /* check collision again */
            for(j=0; j < num_tanks; j++){
                if(   s->x >= tanks[j]->x - 40
                   && s->x <= tanks[j]->x + 40
                   && s->y >= tanks[j]->y - 40
                   && s->y <= tanks[j]->y + 40
               && tanks[j]->health > 0){
                    /* hit! */
                    add_event(BOTS_EVENT_HIT, j);

                    /* record damage */
                    tanks[j]->health -= 10;
                    if(tanks[j]->health <= 0) {
                        tanks[j]->health = 0;
                        add_event(BOTS_EVENT_DEATH, j);
                    }

                    /* delete the shot */
                    free(s);
                    int k;
                    for(k=i; shots[k]; k++){
                        shots[k] = shots[k+1];
                    }
                    i--;
                }
            }
        }
        /* bots */
        for(i=0; i < num_tanks; i++){
            if(tanks[i]->health <= 0)
                continue;
            /* turn, etc */
            short real_steering;
            short real_turret_steering;
            short real_scanner_steering;
            uint16_t steering;
            uint16_t turret_steering;
            uint16_t scanner_steering;
            uint16_t steering_adjust;

            steering = tanks[i]->_req_steering;

            real_steering = steering;
            if(real_steering <= 512 && real_steering > options.hull_turn_rate){
                real_steering = options.hull_turn_rate;
            }
            if(real_steering > 512 && real_steering < (1024 - options.hull_turn_rate)){
                real_steering = 1024 - options.hull_turn_rate;
            }
            tanks[i]->_req_steering -= real_steering;

            tanks[i]->heading = (tanks[i]->heading + real_steering) % 1024;
            tanks[i]->speed = tanks[i]->_req_throttle;
            if(tanks[i]->speed > 100)
                tanks[i]->speed = 100;

            /* drive! */

            double rangle = (tanks[i]->heading) * M_PI / 512;
            double dist = (tanks[i]->speed / 100.0) * 6.0;
            int dy = floor(0.5 + (dist * cos(rangle)));
            int dx = floor(0.5 + (dist * sin(rangle)));
            tanks[i]->x += dx;
            tanks[i]->y += dy;

            /* turn turret */
            turret_steering = tanks[i]->_req_turret_steering;
            if(tanks[i]->_req_turret_keepshift) /* turret keepshift */
                turret_steering = (turret_steering + 1024 - real_steering) % 1024;
            turret_steering = turret_steering % 1024;

            real_turret_steering = turret_steering;
            if(real_turret_steering <= 512 && real_turret_steering > options.turret_turn_rate){
                real_turret_steering = options.turret_turn_rate;
            }
            if(real_turret_steering > 512 && real_turret_steering < (1024 - options.turret_turn_rate)){
                real_turret_steering = 1024 - options.turret_turn_rate;
            }
            tanks[i]->_req_turret_steering -= real_turret_steering;

            tanks[i]->turret_offset = (tanks[i]->turret_offset + real_turret_steering) % 1024;

            /* turn scanner */
            scanner_steering = tanks[i]->_req_scanner_steering;
            if(tanks[i]->_req_scanner_keepshift) /* scanner keepshift */
                scanner_steering = (scanner_steering + 1024 - real_steering) % 1024;
            scanner_steering = scanner_steering % 1024;

            real_scanner_steering = scanner_steering;
            if(real_scanner_steering <= 512 && real_scanner_steering > options.scanner_turn_rate) {
                real_scanner_steering = options.scanner_turn_rate;
            }
            if(real_scanner_steering > 512 && real_scanner_steering < (1024 - options.scanner_turn_rate)) {
                real_scanner_steering = 1024 - options.scanner_turn_rate;
            }
            tanks[i]->_req_scanner_steering -= real_scanner_steering;

            tanks[i]->scanner_offset = (tanks[i]->scanner_offset + real_scanner_steering) % 1024;
        }

    }

    void World::_process_tick() {
        int i;
        for(i=0; i < num_tanks; i++){
            if(tanks[i]->health <= 0)
                continue;

            /** write I/O ports to CPU **/
            for(int j=0; tanks[i]->peripherals[j].mem_base != 0; j++)
                tanks[i]->peripherals[j].process_tick(
                        tanks[i]->peripherals + j, this, i, 1);

            /** run CPU cycles **/
            for(int j=0; j<options.cpus_per_tick; j++)
                bots_cpu_cycle(cpus[i]);

            /** read I/O ports from CPU **/
            for(int j=0; tanks[i]->peripherals[j].mem_base != 0; j++)
                tanks[i]->peripherals[j].process_tick(
                        tanks[i]->peripherals + j, this, i, 0);
        }
    }

    bots_events *World::tick() {
        _tick_events->event_count = 0;

        _physics_tick();
        _process_tick();

        return _tick_events;
    }

    void World::add_bot(bots_cpu *m, bots_tank *p) {
        if(num_tanks == BOTS_MAX_COUNT){
            return;
        }
        m->bot_id = num_tanks;
        cpus[num_tanks] = m;
        tanks[num_tanks] = p;
        num_tanks++;
    }

    void World::place_bots() {
        int32_t spawn_d = options.spawn_distance;

        double angle = 0;
        double step = (2 * M_PI) / num_tanks;

        for(int i=0; i<num_tanks; i++) {
            /* set bot to location */
            int32_t loc_x = floor(0.5 + (spawn_d * cos(angle)));
            int32_t loc_y = floor(0.5 + (spawn_d * sin(angle)));
            tanks[i]->x = loc_x;
            tanks[i]->y = loc_y;

            /* rotate location to next spawn location */
            angle += step;
        }
    }
}


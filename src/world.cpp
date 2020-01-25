#include <bots/cpu.hpp>
#include <bots/world.hpp>

#include <bots/peripherals.hpp>

#include <iostream>
#include <fstream>
#include <iterator>
#include <cstdlib>
#include <cstring>
#include <cmath>

namespace bots {
    // class Bot
    std::unique_ptr<Bot> Bot::build(World &world, std::vector<uint8_t> &data) {
        auto cpu = std::make_unique<Cpu>();
        auto tank = std::make_unique<Tank>();

        tank->health = 100;
        cpu->user_mem_max = 0xefff;
        cpu->registers[10] = 0xefff;
        memcpy(cpu->memory, &data[0], data.size());

        bots_peripheral *peripherals = (bots_peripheral*)calloc(5, sizeof(bots_peripheral));
        peripherals[0].mem_base = 0xfef0;
        peripherals[0].process_tick = &bots_peripheral_reset;
        peripherals[1].mem_base = 0xfee0;
        peripherals[1].process_tick = &bots_peripheral_radar;
        peripherals[2].mem_base = 0xfed0;
        peripherals[2].process_tick = &bots_peripheral_turret;
        peripherals[3].mem_base = 0xfec0;
        peripherals[3].process_tick = &bots_peripheral_hull;

        return std::make_unique<Bot>(world, std::move(cpu), std::move(tank), peripherals);
    }

    std::unique_ptr<Bot> Bot::build(World &world, std::istream &handle) {
        std::vector<uint8_t> data;
        uint8_t byte;
        while(byte = handle.get(), handle.good()) {
            data.push_back(byte);
        }
        // I want to do this, but it skips "whitespace" bytes...
        //std::copy(
        //    std::istream_iterator<uint8_t>(handle),
        //    std::istream_iterator<uint8_t>(),
        //    std::back_inserter(data)
        //);
        return build(world, data);
    }

    std::unique_ptr<Bot> Bot::build(World &world, const std::string &filename) {
        std::ifstream f{filename, std::ios::in | std::ios::binary};
        return build(world, f);
    }
}

namespace bots {
    // class World
    World::World(Options options = {}) : options(options) {}
    World::~World() {}

    void World::add_event(World::Event::Type event_type, uint8_t bot_id) {
        tick_events.push_back(Event{.type = event_type, .bot = *bots[bot_id], .shot = {}});
    }

    void World::_physics_tick() {
        /** run world physics **/
        /* shots */
        auto s = shots.begin();
        while(s != shots.end()) {
            /* check collision */
            int hit = 0;
            int j;
            for(j=0; j < bots.size(); j++){
                if(   s->x >= bots[j]->tank->x - 40
                   && s->x <= bots[j]->tank->x + 40
                   && s->y >= bots[j]->tank->y - 40
                   && s->y <= bots[j]->tank->y + 40
                   && bots[j]->tank->health > 0
                ){
                    /* hit! */
                    hit = 1;
                    add_event(World::Event::Type::hit, j);

                    /* record damage */
                    bots[j]->tank->health -= 10;
                    if(bots[j]->tank->health <= 0) {
                        bots[j]->tank->health = 0;
                        add_event(World::Event::Type::death, j);
                    }

                    /* delete the shot */
                    s = shots.erase(s);
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
                s = shots.erase(s);
                continue;
            }

            /* check collision again */
            for(j=0; j < bots.size(); j++){
                if(   s->x >= bots[j]->tank->x - 40
                   && s->x <= bots[j]->tank->x + 40
                   && s->y >= bots[j]->tank->y - 40
                   && s->y <= bots[j]->tank->y + 40
                   && bots[j]->tank->health > 0
                ){
                    /* hit! */
                    hit = 1;
                    add_event(World::Event::Type::hit, j);

                    /* record damage */
                    bots[j]->tank->health -= 10;
                    if(bots[j]->tank->health <= 0) {
                        bots[j]->tank->health = 0;
                        add_event(World::Event::Type::death, j);
                    }

                    /* delete the shot */
                    s = shots.erase(s);
                }
            }
            if (hit)
                continue;

            s++;
        }
        /* bots */
        for(int i=0; i < bots.size(); i++){
            if(bots[i]->tank->health <= 0)
                continue;
            /* turn, etc */
            short real_steering;
            short real_turret_steering;
            short real_scanner_steering;
            uint16_t steering;
            uint16_t turret_steering;
            uint16_t scanner_steering;
            uint16_t steering_adjust;

            steering = bots[i]->tank->_req_steering;

            real_steering = steering;
            if(real_steering <= 512 && real_steering > options.hull_turn_rate){
                real_steering = options.hull_turn_rate;
            }
            if(real_steering > 512 && real_steering < (1024 - options.hull_turn_rate)){
                real_steering = 1024 - options.hull_turn_rate;
            }
            bots[i]->tank->_req_steering -= real_steering;

            bots[i]->tank->heading = (bots[i]->tank->heading + real_steering) % 1024;
            bots[i]->tank->speed = bots[i]->tank->_req_throttle;
            if(bots[i]->tank->speed > 100)
                bots[i]->tank->speed = 100;

            /* drive! */

            double rangle = (bots[i]->tank->heading) * M_PI / 512;
            double dist = (bots[i]->tank->speed / 100.0) * 6.0;
            int dy = floor(0.5 + (dist * cos(rangle)));
            int dx = floor(0.5 + (dist * sin(rangle)));
            bots[i]->tank->x += dx;
            bots[i]->tank->y += dy;

            /* turn turret */
            turret_steering = bots[i]->tank->_req_turret_steering;
            if(bots[i]->tank->_req_turret_keepshift) /* turret keepshift */
                turret_steering = (turret_steering + 1024 - real_steering) % 1024;
            turret_steering = turret_steering % 1024;

            real_turret_steering = turret_steering;
            if(real_turret_steering <= 512 && real_turret_steering > options.turret_turn_rate){
                real_turret_steering = options.turret_turn_rate;
            }
            if(real_turret_steering > 512 && real_turret_steering < (1024 - options.turret_turn_rate)){
                real_turret_steering = 1024 - options.turret_turn_rate;
            }
            bots[i]->tank->_req_turret_steering -= real_turret_steering;

            bots[i]->tank->turret_offset = (bots[i]->tank->turret_offset + real_turret_steering) % 1024;

            /* turn scanner */
            scanner_steering = bots[i]->tank->_req_scanner_steering;
            if(bots[i]->tank->_req_scanner_keepshift) /* scanner keepshift */
                scanner_steering = (scanner_steering + 1024 - real_steering) % 1024;
            scanner_steering = scanner_steering % 1024;

            real_scanner_steering = scanner_steering;
            if(real_scanner_steering <= 512 && real_scanner_steering > options.scanner_turn_rate) {
                real_scanner_steering = options.scanner_turn_rate;
            }
            if(real_scanner_steering > 512 && real_scanner_steering < (1024 - options.scanner_turn_rate)) {
                real_scanner_steering = 1024 - options.scanner_turn_rate;
            }
            bots[i]->tank->_req_scanner_steering -= real_scanner_steering;

            bots[i]->tank->scanner_offset = (bots[i]->tank->scanner_offset + real_scanner_steering) % 1024;
        }

    }

    void World::_process_tick() {
        int i;
        for(i=0; i < bots.size(); i++){
            if(bots[i]->tank->health <= 0)
                continue;

            /** write I/O ports to CPU **/
            for(int j=0; bots[i]->peripherals[j].mem_base != 0; j++)
                bots[i]->peripherals[j].process_tick(
                        bots[i]->peripherals + j, this, i, 1);

            /** run CPU cycles **/
            for(int j=0; j<options.cpus_per_tick; j++)
                bots[i]->cpu->cycle();

            /** read I/O ports from CPU **/
            for(int j=0; bots[i]->peripherals[j].mem_base != 0; j++)
                bots[i]->peripherals[j].process_tick(
                        bots[i]->peripherals + j, this, i, 0);
        }
    }

    std::vector<World::Event> World::tick() {
        tick_events.clear();

        _physics_tick();
        _process_tick();

        return tick_events;
    }

    void World::place_bots() {
        int32_t spawn_d = options.spawn_distance;

        double angle = 0;
        double step = (2 * M_PI) / bots.size();

        for(int i=0; i<bots.size(); i++) {
            /* set bot to location */
            int32_t loc_x = floor(0.5 + (spawn_d * cos(angle)));
            int32_t loc_y = floor(0.5 + (spawn_d * sin(angle)));
            bots[i]->tank->x = loc_x;
            bots[i]->tank->y = loc_y;

            /* rotate location to next spawn location */
            angle += step;
        }
    }
}


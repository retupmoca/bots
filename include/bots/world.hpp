#pragma once

#include <cstdint>

#include <bots/struct.h>

#include <include/bots/tank>

namespace bots {
    class Shot {
        int32_t x;
        int32_t y;
        uint32_t heading;

        Tank &from;
        long long id;
    };

    class World {
        public:
            struct Options {
                uint8_t cpus_per_tick = 2;
                int32_t spawn_distance = 100;

                uint16_t hull_turn_rate = 1;
                uint16_t turret_turn_rate = 2;
                uint16_t scanner_turn_rate = 8;
            };
            class Event {
                uint8_t type;
                Tank &bot;
                Shot &shot;
            };

            Options options;

            uint8_t num_tanks;

            std::vector<Tank> tanks;
            std::list<Shot> shots;
            
            World(std::vector<Tank> tanks, Options options = Options{});
            ~World();

            std::vector<Event> tick();

        private:
            long long next_shot_id;
            std::vector<Event> _tick_events;
    };
}

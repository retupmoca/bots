#pragma once

#include <vector>
#include <memory>
#include <cstdint>

#include <bots/struct.hpp>

namespace bots {
    //class Shot {
    //    int32_t x;
    //    int32_t y;
    //    uint32_t heading;

    //    Tank &from;
    //    long long id;
    //};
    class World;

    class Bot {
        public:
        World &world;
        bots_cpu *cpu;
        bots_tank *tank;

        static std::unique_ptr<Bot> build(World &world, const std::string &filename);
        static std::unique_ptr<Bot> build(World &world, std::istream &handle);
        static std::unique_ptr<Bot> build(World &world, std::vector<uint8_t> &data);

        Bot(World &world, bots_cpu *cpu, bots_tank *tank) : world(world), cpu(cpu), tank(tank) {}
        ~Bot() {
            free(cpu);
            free(tank);
        }
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
            //class Event {
            //    uint8_t type;
            //    Tank &bot;
            //    Shot &shot;
            //};

            Options options;

            std::vector<std::unique_ptr<Bot>> bots;
            //std::list<Shot> shots;
            
            //World(std::vector<Tank> tanks, Options options = {});
            World(Options options);
            ~World();

            template<typename ...Targs>
            void add_bot(Targs...Fargs) {
                bots.push_back(Bot::build(*this, Fargs...));
            };

            //std::vector<Event> tick();

            //uint8_t num_tanks;

            /*bots_cpu *cpus[BOTS_MAX_COUNT];
            bots_tank *tanks[BOTS_MAX_COUNT];*/
            bots_shot *shots[BOTS_SHOTS_MAX_COUNT];

            bots_events *tick();
            void _physics_tick();
            void _process_tick();
            void add_event(uint8_t, uint8_t);
            //void add_bot(bots_cpu*, bots_tank*);
            void place_bots();

        //private:
            long long next_shot_id;
            //std::vector<Event> tick_events;
            bots_events *_tick_events;
    };
}

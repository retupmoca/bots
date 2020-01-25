#pragma once

#include <optional>
#include <list>
#include <vector>
#include <memory>
#include <cstdint>

#include <bots/struct.hpp>

namespace bots {
    class World;

    class Tank {
        public:
            int32_t x;
            int32_t y;

            uint32_t heading;
            int32_t speed;
            uint32_t turret_offset;
            uint32_t scanner_offset;

            uint8_t health;

            uint16_t _req_steering;
            int16_t _req_throttle;
            uint16_t _req_turret_steering;
            uint8_t _req_turret_keepshift;
            uint16_t _req_scanner_steering;
            uint8_t _req_scanner_keepshift;
    };

    class Bot {
        public:
            World &world;
            std::unique_ptr<Tank> tank;

            bots_cpu *cpu;
            bots_peripheral *peripherals;

            static std::unique_ptr<Bot> build(World &world, const std::string &filename);
            static std::unique_ptr<Bot> build(World &world, std::istream &handle);
            static std::unique_ptr<Bot> build(World &world, std::vector<uint8_t> &data);

            Bot(World &world, bots_cpu *cpu, std::unique_ptr<Tank> tank, bots_peripheral *peripherals)
            : world(world), cpu(cpu), tank(std::move(tank)), peripherals(peripherals)
            {}

            ~Bot() {
                free(cpu);
                free(peripherals);
            }
    };

    struct Shot {
        int32_t x;
        int32_t y;
        uint32_t heading;

        Bot &from;
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
            struct Event {
                enum class Type { nothing, fire, death, scan, hit };

                Type type;
                Bot &bot;
                std::optional<Shot> shot;
            };

            Options options;

            std::vector<std::unique_ptr<Bot>> bots;
            std::list<Shot> shots;
            
            //World(std::vector<Tank> tanks, Options options = {});
            World(Options options);
            ~World();

            template<typename ...Targs>
            void add_bot(Targs...Fargs) {
                bots.push_back(Bot::build(*this, Fargs...));
            };

            std::vector<Event> tick();

            void add_event(Event::Type, uint8_t);
            void place_bots();

            long long next_shot_id;

        private:
            std::vector<Event> tick_events;

            void _physics_tick();
            void _process_tick();
    };
}

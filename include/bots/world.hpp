#pragma once

#include <list>
#include <vector>
#include <memory>
#include <cstdint>

#include <bots/cpu.hpp>

namespace bots {
    class World;
    class Shot;

    class PhysicsObject {
        public:
            World &world;
            
            // location
            int32_t x = 0;
            int32_t y = 0;

            // size
            int32_t w = 0;
            int32_t h = 0;

            // speed vector
            int32_t sx = 0;
            int32_t sy = 0;

            // facing vector
            int32_t fx = 0;
            int32_t fy = 0;

            bool moves = true;

            virtual void physics_tick() = 0;
            virtual void shot(Shot&) = 0;

            PhysicsObject(World &w);

            std::vector<PhysicsObject&> collisions();
    };

    class Wall : public PhysicsObject {
        Wall(World &w) : PhysicsObject(w) {
            moves = false;
        }

        // walls don't do anything
        void physics_tick() override {}
        //void shot(Shot &s) override {}
    };

    class Tank : public PhysicsObject {
        public:
            //int32_t x;
            //int32_t y;
            //uint32_t heading;
            //int32_t speed;

            uint32_t turret_offset;
            uint32_t scanner_offset;

            uint8_t health;

            uint16_t _req_steering;
            int16_t _req_throttle;
            uint16_t _req_turret_steering;
            uint8_t _req_turret_keepshift;
            uint16_t _req_scanner_steering;
            uint8_t _req_scanner_keepshift;

            void physics_tick() override;
    };

    class Bot {
        public:
            World &world;
            std::unique_ptr<Tank> tank;
            std::unique_ptr<Cpu> cpu;

            static std::unique_ptr<Bot> build(World &world, const std::string &filename);
            static std::unique_ptr<Bot> build(World &world, std::istream &handle);
            static std::unique_ptr<Bot> build(World &world, std::vector<uint8_t> &data);

            Bot(World &world, std::unique_ptr<Cpu> cpu, std::unique_ptr<Tank> tank)
            : world(world), cpu(std::move(cpu)), tank(std::move(tank))
            {}
    };

    class Shot : public PhysicsObject {
        //int32_t x;
        //int32_t y;
        //uint32_t heading;

        Bot &from;
        long long id;

        void physics_tick() override;
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
                const Shot *shot;
            };

            Options options;

            std::vector<std::unique_ptr<Bot>> bots;
            std::list<Shot> shots;

            std::list<PhysicsObject*> physics_objects;
            
            //World(std::vector<Tank> tanks, Options options = {});
            World(Options options);
            ~World();

            template<typename ...Targs>
            void add_bot(Targs...Fargs) {
                bots.push_back(Bot::build(*this, Fargs...));
            };

            std::vector<Event> tick();

            void add_event(Event::Type, Bot&);
            void add_event(Event::Type, uint8_t);
            void place_bots();

            long long next_shot_id;

        private:
            std::vector<Event> tick_events;

            void _physics_tick();
            void _process_tick();
    };
}

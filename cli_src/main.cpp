#include <iostream>
#include <fmt/format.h>
#include <chrono>
#include <thread>

#include <bots/world.hpp>

#include <bots/bots.hpp>

int main(int argc, char *argv[]) {
    bots::World g{{.cpus_per_tick = 1}};

    if(argc == 3) {
        if(   !bots::add_bot_from_file(&g, argv[1])
           || !bots::add_bot_from_file(&g, argv[2])) {
            std::cout << "Unable to load bots\n";
            return 1;
        }
    }
    else {
        std::cout << "Please provide two robots\n";
    }

    g.place_bots();

    while(1) {
        g.tick();
        std::cout << fmt::format("Bot 1 heading: {}\n", g.tanks[0]->heading);
        std::cout << fmt::format("Bot 1 position: {}:{}\n", g.tanks[0]->x, g.tanks[0]->y);
        std::cout << fmt::format("Bot 2 position: {}:{}\n", g.tanks[1]->x, g.tanks[1]->y);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return 0;
}

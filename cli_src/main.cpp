#include <iostream>
#include <fmt/format.h>
#include <chrono>
#include <thread>

#include <bots.hpp>

int main(int argc, char *argv[]) {
    bots::World g{{.cpus_per_tick = 1}};

    if(argc == 3) {
        g.add_bot(argv[1]);
        g.add_bot(argv[2]);
    }
    else {
        std::cout << "Please provide two robots\n";
        return 1;
    }

    g.place_bots();

    while(1) {
        g.tick();
        std::cout << fmt::format("Bot 1 heading: {}\n", g.bots[0]->tank->heading);
        std::cout << fmt::format("Bot 1 position: {}:{}\n", g.bots[0]->tank->x, g.bots[0]->tank->y);
        std::cout << fmt::format("Bot 2 position: {}:{}\n", g.bots[1]->tank->x, g.bots[1]->tank->y);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return 0;
}

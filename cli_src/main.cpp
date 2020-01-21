#include <iostream>
#include <fmt/format.h>
#include <chrono>
#include <thread>

#include <bots/bots.h>

int main(int argc, char *argv[]) {
    bots_world *g = bots_create_world();

    if(argc == 3) {
        if(   !bots_add_bot_from_file(g, argv[1])
           || !bots_add_bot_from_file(g, argv[2])) {
            std::cout << "Unable to load bots\n";
            return 1;
        }
    }
    else {
        std::cout << "Please provide two robots\n";
    }

    bots_spawn(g);

    g->c_cpus_per_tick = 1;

    while(1) {
        bots_tick(g);
        std::cout << fmt::format("Bot 1 heading: {}\n", g->tanks[0]->heading);
        printf("Bot 1 position: %i:%i\n", g->tanks[0]->x, g->tanks[0]->y);
        std::cout << fmt::format("Bot 1 position: {}:{}\n", g->tanks[0]->x, g->tanks[0]->y);
        std::cout << fmt::format("Bot 2 position: {}:{}\n", g->tanks[1]->x, g->tanks[1]->y);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    bots_free_world(g);
    return 0;
}

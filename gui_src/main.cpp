#include <iostream>
#include <fmt/format.h>

#include <bots.hpp>
#include "window.hpp"
#include "engine.hpp"

class BotsEngine : public llm::Engine {
public:
    bots::World *world;

    BotsEngine() {
        tickRate = 1 / 60.0f;
    }
};

class BotsWindow : public llm::PixelDrawingWindow {
public:
    BotsEngine *game;

    BotsWindow() {
        title = "Bots";
        width = 640;
        height = 480;
        vsync = true;
    }
};

int main(int argc, char *argv[]) {
    bots::World world{{}};

    BotsEngine engine;
    BotsWindow window;

    engine.world = &world;
    window.game = &engine;

    if(argc == 3) {
        world.add_bot(argv[1]);
        world.add_bot(argv[2]);
    }
    else {
        std::cout << "Please provide two robots\n";
        return 1;
    }

    world.place_bots();

    engine.start();
    window.start();

    window.wait();

    return 0;
}

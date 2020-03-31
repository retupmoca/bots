#include <iostream>
#include <fmt/format.h>

#include <bots.hpp>
#include "window.hpp"
#include "engine.hpp"

class BotsEngine : public llm::Engine {
public:
    bots::World *world;

    void tick() override {
        world->tick();
    }

    BotsEngine() {
        tickRate = 1 / 60.0f;
    }
};

class BotsWindow : public llm::PixelDrawingWindow {
public:
    BotsEngine *game;

    void tick() override {
        for(int x = 0; x < width; x++)
            for(int y = 0; y < height; y++)
                at(x, y).set(0, 0, 0);
        bots::World *w = game->world;

        int botax = w->bots[0]->tank->x/16 + width/2;
        int botay = w->bots[0]->tank->y/16 + height/2;

        if (w->bots[0]->tank->health > 0 && botax >= 0 && botax < width && botay >= 0 && botay < height)
            at(botax, botay).set(255, 255, 255);

        int botbx = w->bots[1]->tank->x/16 + width/2;
        int botby = w->bots[1]->tank->y/16 + height/2;

        if (w->bots[1]->tank->health > 0 && botbx >= 0 && botbx < width && botby >= 0 && botby < height)
            at(botbx, botby).set(255, 255, 255);

    }

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

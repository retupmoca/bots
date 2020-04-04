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

class BotsWindow : public llm::FramebufferWindow {
public:
    BotsEngine *game;

    void tick() override {
        frame->clear(llm::color::BLACK);
        bots::World *w = game->world;

        const int32_t SCALE = 8;

        int botax = w->bots[0]->tank->x/SCALE + frame->width/2;
        int botay = w->bots[0]->tank->y/SCALE + frame->height/2;

        frame->rectDraw(botax - 3, botay - 3, 7, 7, llm::color::WHITE);

        int botbx = w->bots[1]->tank->x/SCALE + frame->width/2;
        int botby = w->bots[1]->tank->y/SCALE + frame->height/2;

        frame->rectDraw(botbx - 3, botby - 3, 7, 7, llm::color::WHITE);

        for (auto &shot : w->shots) {
            int sx = shot.x/SCALE + frame->width/2;
            int sy = shot.y/SCALE + frame->height/2;
            frame->rectDraw(sx-1, sy-1, 3, 3, llm::color::RED);
        }
    }

    BotsWindow() {
        title = "Bots";
        frame = new llm::Framebuffer(960, 540);
        vsync = true;
        showFPS = false;
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

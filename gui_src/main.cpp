#include <iostream>
#include <fmt/format.h>
#include <mutex>
#include <queue>

#include <bots.hpp>
#include "window.hpp"
#include "engine.hpp"

class BotsEngine : public llm::Engine {
public:
    bots::World *world;

    std::mutex displayLock;
    std::queue<bots::World::Event> displayEvents;

    void tick() override {
        auto tickEvents = world->tick();

        displayLock.lock();
        for (auto &event : tickEvents) {
            displayEvents.push(event);
        }
        displayLock.unlock();
    }

    BotsEngine() {
        tickRate = 1 / 60.0f;
    }
};

class BotsWindow : public llm::FramebufferWindow {
public:
    BotsEngine *game;
    llm::Framebuffer *tankSprite;
    llm::Framebuffer *turretSprite;

    void tick() override {
        frame->clear(llm::color::BLACK);
        bots::World *w = game->world;

        const int32_t SCALE = 8;

        for (auto &bot : w->bots) {
            int botx = bot->tank->x/SCALE + frame->width/2;
            int boty = bot->tank->y/SCALE + frame->height/2;

            //frame->rectDraw(botx - 3, boty - 3, 7, 7, llm::color::WHITE);
            using llm::Affine;

            Affine t;
            t.add(Affine::Translate(-480, -205));
            //t.add(Affine::Rotate(M_PI));
            double heading_rad = (bot->tank->heading) * M_PI / 512;
            t.add(Affine::Rotate(heading_rad));
            t.add(Affine::Scale(0.1));
            t.add(Affine::Translate(botx, boty));
            frame->blit(tankSprite, t);

            Affine t2;
            t2.add(Affine::Translate(-480, -205));
            //t.add(Affine::Rotate(M_PI));
            double theading_rad = (bot->tank->turret_offset + bot->tank->heading) * M_PI / 512;
            t2.add(Affine::Rotate(theading_rad));
            t2.add(Affine::Scale(0.1));
            t2.add(Affine::Translate(botx, boty));
            frame->blit(turretSprite, t2);
        }

        for (auto &shot : w->shots) {
            int sx = shot.x/SCALE + frame->width/2;
            int sy = shot.y/SCALE + frame->height/2;
            frame->rectDraw(sx-1, sy-1, 3, 3, llm::color::RED);
        }

        game->displayLock.lock();
        while(!game->displayEvents.empty()) {
            auto event = game->displayEvents.front();
            game->displayEvents.pop();

            // do a thing with event
        }
        game->displayLock.unlock();
    }

    BotsWindow() {
        title = "Bots";
        frame = new llm::Framebuffer(960, 540);
        tankSprite = new llm::Framebuffer("gui_assets/tankbody.png");
        turretSprite = new llm::Framebuffer("gui_assets/tankturret.png");
        vsync = true;
        showFPS = false;
    }

    ~BotsWindow() {
        delete frame;
        delete tankSprite;
        delete turretSprite;
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

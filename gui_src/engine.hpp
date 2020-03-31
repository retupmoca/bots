/* rev: f210665fff562910225a5ff97d675d91985214fc */
#include <thread>

namespace llm {
    class Engine {
    private:
        std::thread *thread = nullptr;
        static void threadMain(Engine *engine) {
            engine->mainLoop();
        }

        void mainLoop() {
            auto tp1 = std::chrono::system_clock::now();
            auto tp2 = tp1;
            std::chrono::duration<float> target(tickRate);
            std::chrono::duration<float> accumulated(0);

            while(active) {
                tp2 = std::chrono::system_clock::now();
                std::chrono::duration<float> elapsed = tp2 - tp1;
                tp1 = tp2;

                accumulated -= elapsed;
                accumulated += target;

                if (accumulated.count() > 0)
                    std::this_thread::sleep_for(accumulated);

                tick();
            }
        }

    public:
        bool active = true;
        float tickRate = 1;

        Engine() { }

        ~Engine() {
            active = false;
            wait();
        }

        void start() {
            thread = new std::thread(threadMain, this);
        }

        void wait() {
            if (thread) {
                thread->join();
                delete thread;
                thread = nullptr;
            }
        }

        virtual void tick() { }
    };
};

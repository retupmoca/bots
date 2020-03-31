/* rev: f210665fff562910225a5ff97d675d91985214fc */
#include <GLFW/glfw3.h>
#include <string>
#include <chrono>
#include <fmt/format.h>
#include <thread>

namespace llm {
    struct pixel {
        uint8_t r, g, b;

        void set(uint8_t newR, uint8_t newG, uint8_t newB) {
            r = newR; g = newG; b = newB;
        }

        void set(const pixel &from) {
            r = from.r; g = from.g; b = from.b;
        }
    };

    class FramebufferWindow {
    private:
        GLuint glBuffer;
        std::thread *thread = nullptr;

        static void threadMain(FramebufferWindow *window) {
            window->mainLoop();
        }

        static void rawKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
            FramebufferWindow *me = (FramebufferWindow*)glfwGetWindowUserPointer(window);
            me->keyboardEvent(key, scancode, action, mods);
        }

        virtual void keyboardEvent(int key, int scancode, int action, int mods) { }

        void mainLoop() {
            glfwInit();
         
            window = glfwCreateWindow(width * defaultScale, height * defaultScale, title.c_str(), NULL, NULL);
            glfwSetWindowUserPointer(window, this);
         
            glfwSetKeyCallback(window, rawKeyCallback);
         
            glfwMakeContextCurrent(window);
            glfwSwapInterval(vsync ? 1 : 0);
            
            glEnable(GL_TEXTURE_2D);
            glGenTextures(1, &glBuffer);
            glBindTexture(GL_TEXTURE_2D, glBuffer);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, framebuffer);

            auto tp1 = std::chrono::system_clock::now();
            auto tp2 = std::chrono::system_clock::now();

            while (!glfwWindowShouldClose(window) && active) {
                tp2 = std::chrono::system_clock::now();
                std::chrono::duration<float> elapsed = tp2 - tp1;
                float fps = 1 / elapsed.count();
                tp1 = tp2;
                tick(elapsed.count());

                if (!glfwGetWindowAttrib(window, GLFW_ICONIFIED) && glfwGetWindowAttrib(window, GLFW_VISIBLE)) {
                    float ratio, bufRatio;
                    int winWidth, winHeight;
             
                    glClear(GL_COLOR_BUFFER_BIT);
                    glfwGetFramebufferSize(window, &winWidth, &winHeight);
                    if (scaleUp || winWidth < width || winHeight < height) {
                        ratio = winWidth / (float) winHeight;
                        bufRatio = width / (float) height;
                 
                        if (bufRatio == ratio)
                            glViewport(0, 0, winWidth, winHeight);
                        if (bufRatio > ratio) {
                            float fakeHeight = winWidth / bufRatio;
                            glViewport(0, (winHeight - fakeHeight) / 2, winWidth, fakeHeight);
                        }
                        if (bufRatio < ratio) {
                            float fakeWidth = winHeight * bufRatio;
                            glViewport((winWidth - fakeWidth) / 2, 0, fakeWidth, winHeight);
                        }
                    }
                    else {
                        glViewport((winWidth - width) / 2, (winHeight - height) / 2, width, height);
                    }
             
                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, framebuffer);

                    glBegin(GL_QUADS);
                        glTexCoord2f(0.0, 1.0); glVertex3f(-1.0f, -1.0f, 0.0f);
                        glTexCoord2f(0.0, 0.0); glVertex3f(-1.0f,  1.0f, 0.0f);
                        glTexCoord2f(1.0, 0.0); glVertex3f( 1.0f,  1.0f, 0.0f);
                        glTexCoord2f(1.0, 1.0); glVertex3f( 1.0f, -1.0f, 0.0f);
                    glEnd();
             
                    glfwSwapBuffers(window);
                    glfwSetWindowTitle(window, fmt::format("{} - {} FPS", title, (int)fps).c_str());
                }
                glfwPollEvents();
            }

            if (window)
                glfwDestroyWindow(window);
            glfwTerminate();
        }

    public:
        std::string title = "FramebufferWindow";
        pixel *framebuffer = nullptr;
        GLFWwindow *window = nullptr;
        int width = 0, height = 0;
        float defaultScale = 1.0f;
        bool vsync = false;
        bool scaleUp = true;
        bool active = true;

        FramebufferWindow() { }

        void start() {
            framebuffer = new pixel[width * height];
            for(int i = 0; i < width * height; i++)
                framebuffer[i].set(0, 0, 0);

            thread = new std::thread(threadMain, this);
        }

        void wait() {
            if (thread) {
                thread->join();
                delete thread;
                thread = nullptr;
            }
        }

        ~FramebufferWindow() {
            active = false;
            wait();
            if (framebuffer)
                delete framebuffer;
        }

        virtual void tick(float elapsedTime) { tick(); }
        virtual void tick() { }
    };

    class PixelDrawingWindow : public FramebufferWindow {
    public:
        pixel& at(int x, int y) {
            return framebuffer[x + y*width];
        }
    };
};

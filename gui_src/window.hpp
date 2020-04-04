/* rev f6dd658ae7d6ffe1ef7bef76f9f09f068d7f7901 */
#include <GLFW/glfw3.h>
#include <string>
#include <chrono>
#include <fmt/format.h>
#include <thread>
#include <algorithm>

namespace llm {
    struct pixel {
        uint8_t r, g, b, a = 0xff;

        void set(uint8_t newR, uint8_t newG, uint8_t newB, uint8_t newA = 0xff) {
            r = newR; g = newG; b = newB; a = newA;
        }

        void set(const pixel &from) {
            r = from.r; g = from.g; b = from.b; a = from.a;
        }
    };

    namespace color {
        const pixel CLEAR = {0x0, 0x0, 0x0, 0x0};
        const pixel WHITE = {0xff, 0xff, 0xff};
        const pixel BLACK = {0x0, 0x0, 0x0};
        const pixel RED = {0xff, 0x0, 0x0};
        const pixel GREEN = {0x0, 0xff, 0x0};
        const pixel BLUE = {0x0, 0x0, 0xff};
    };

    class Framebuffer {
    public:
        pixel *pixels = nullptr;
        int width = 0, height = 0;

        Framebuffer(int width, int height) {
            this->width = width;
            this->height = height;
            pixels = new pixel[width * height];
        }

        ~Framebuffer() {
            if (pixels)
                delete pixels;
        }

        pixel& at(int x, int y) {
            return pixels[x + y*width];
        }

        void clear(pixel p) {
            std::fill(pixels, pixels + width * height, p);
        }

        bool _clipRect(int &x, int &y, int &w, int &h) {
            // clip the rectangle if it goes off the buffer
            if (x + w < 0 || y + h < 0 || x >= width || y >= height)
                return false;

            if (y < 0) {
                y = 0;
                h += y;
            }
            if (x < 0) {
                x = 0;
                w += x;
            }
            if (x + w >= width)
                w = width - x;
            if (y + h >= height)
                h = height - y;

            return true;
        }

        void rectFill(int x, int y, int w, int h, pixel p) {
            if (_clipRect(x, y, w, h))
                for (int i = y; i < y+h; ++i)
                    std::fill(pixels + x + i*width, pixels + x + w + i*width, p);
        }

        void rectDraw(int x, int y, int w, int h, pixel p) {
            int startX = x, endX = x+w, startY = y, endY = y+h;
            if (_clipRect(x, y, w, h))
                for (int i = y; i < y+h; ++i)
                    if (i == startY || i == endY-1)
                        std::fill(pixels + x + i*width, pixels + x + w + i*width, p);
                    else {
                        if (x == startX)
                            at(x, i).set(p);
                        if (x + w == endX)
                            at(x + w - 1, i).set(p);
                    }
        }
    };

    class FramebufferWindow {
    private:
        GLuint glBuffer;
        std::thread *thread = nullptr;
        bool iconified = false;

        static void threadMain(FramebufferWindow *window) {
            window->mainLoop();
        }

        static void rawKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
            FramebufferWindow *me = (FramebufferWindow*)glfwGetWindowUserPointer(window);
            me->keyboardEvent(key, scancode, action, mods);
        }

        static void rawResizeCallback(GLFWwindow *window, int width, int height) {
            FramebufferWindow *me = (FramebufferWindow*)glfwGetWindowUserPointer(window);
            me->needsResize = true;
        }

        virtual void keyboardEvent(int key, int scancode, int action, int mods) { }

        void mainLoop() {
            glfwInit();
         
            window = glfwCreateWindow(
                frame->width * defaultScale,
                frame->height * defaultScale,
                title.c_str(), NULL, NULL
            );
            glfwSetWindowUserPointer(window, this);
         
            glfwSetKeyCallback(window, rawKeyCallback);
            glfwSetFramebufferSizeCallback(window, rawResizeCallback);
         
            glfwMakeContextCurrent(window);
            glfwSwapInterval(vsync ? 1 : 0);
            
            glEnable(GL_TEXTURE_2D);
            glGenTextures(1, &glBuffer);
            glBindTexture(GL_TEXTURE_2D, glBuffer);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame->width, frame->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame->pixels);

            auto tp1 = std::chrono::system_clock::now();
            auto tp2 = std::chrono::system_clock::now();

            while (!glfwWindowShouldClose(window) && active) {
                tp2 = std::chrono::system_clock::now();
                std::chrono::duration<float> elapsed = tp2 - tp1;
                float fps = 1 / elapsed.count();
                tp1 = tp2;
                tick(elapsed.count());

                if (!glfwGetWindowAttrib(window, GLFW_ICONIFIED) && glfwGetWindowAttrib(window, GLFW_VISIBLE)) {
                    int width = frame->width;
                    int height = frame->height;

                    if (needsResize) {
                        needsResize = false;
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
                    }
             
                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, frame->pixels);

                    glBegin(GL_QUADS);
                        glTexCoord2f(0.0, 1.0); glVertex3f(-1.0f, -1.0f, 0.0f);
                        glTexCoord2f(0.0, 0.0); glVertex3f(-1.0f,  1.0f, 0.0f);
                        glTexCoord2f(1.0, 0.0); glVertex3f( 1.0f,  1.0f, 0.0f);
                        glTexCoord2f(1.0, 1.0); glVertex3f( 1.0f, -1.0f, 0.0f);
                    glEnd();
             
                    glfwSwapBuffers(window);
                    if (showFPS)
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
        Framebuffer *frame = nullptr;
        GLFWwindow *window = nullptr;
        float defaultScale = 1.0f;
        bool vsync = false;
        bool scaleUp = true;
        bool needsResize = true;
        bool active = true;
        bool showFPS = false;

        FramebufferWindow() { }

        void start() {
            if (frame)
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
        }

        virtual void tick(float elapsedTime) { tick(); }
        virtual void tick() { }
    };
};

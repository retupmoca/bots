/* rev 2ee34e61fb891e379c04fab7e363579f9c182585 */
#include <GLFW/glfw3.h>
#include <png.h>
#include <fmt/format.h>

#include <string>
#include <chrono>
#include <thread>
#include <algorithm>

namespace llm {
    struct mat3 {
        float m[3][3];

        mat3 invert() {
            float det = m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
                m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2]) +
                m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);

            float idet = 1.0f / det;
            mat3 matOut;
            matOut.m[0][0] = (m[1][1] * m[2][2] - m[1][2] * m[2][1]) * idet;
            matOut.m[1][0] = (m[2][0] * m[1][2] - m[1][0] * m[2][2]) * idet;
            matOut.m[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * idet;
            matOut.m[0][1] = (m[2][1] * m[0][2] - m[0][1] * m[2][2]) * idet;
            matOut.m[1][1] = (m[0][0] * m[2][2] - m[2][0] * m[0][2]) * idet;
            matOut.m[2][1] = (m[0][1] * m[2][0] - m[0][0] * m[2][1]) * idet;
            matOut.m[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * idet;
            matOut.m[1][2] = (m[0][2] * m[1][0] - m[0][0] * m[1][2]) * idet;
            matOut.m[2][2] = (m[0][0] * m[1][1] - m[0][1] * m[1][0]) * idet;
            return matOut;
        }
    };

    class Affine {
    public:
        // read these sideways-ish
        static mat3 Identity() {
            return {1, 0, 0,
                    0, 1, 0,
                    0, 0, 1};
        }
        static mat3 Translate(float x, float y) {
            return {1, 0, 0,
                    0, 1, 0,
                    x, y, 1};
        }
        static mat3 Rotate(float r) {
            return {cosf(r), -sinf(r), 0,
                    sinf(r), cosf(r), 0,
                    0, 0, 1};
        }
        static mat3 Scale(float s) { return Scale(s, s); }
        static mat3 Scale(float sx, float sy) {
            return {sx, 0, 0,
                    0, sy, 0,
                    0, 0, 1};
        }
        // TODO: skew?

    public:
        mat3 matrix;
        mat3 inverse;
        bool inverseDirty = true;

        Affine() {
            // initialize to the identity transformation
            matrix = Affine::Identity();
        }

        void reset() {
            matrix = Affine::Identity();
            inverseDirty = true;
        }

        void add(mat3 transform) {
            inverseDirty = true;
            // to add a transformation, do a matrix multiply
            mat3 tmp = matrix;
            for (int i=0; i<3; ++i)
                for (int j=0; j<3; ++j)
                    matrix.m[i][j] = transform.m[0][j] * tmp.m[i][0]
                                     + transform.m[1][j] * tmp.m[i][1]
                                     + transform.m[2][j] * tmp.m[i][2];
        }

        void map(int x, int y, int &newX, int &newY) {
            // multiply vector [x, y, 1] by the matrix
            newX = x*matrix.m[0][0] + y*matrix.m[1][0] + 1*matrix.m[2][0] + 0.5;
            newY = x*matrix.m[0][1] + y*matrix.m[1][1] + 1*matrix.m[2][1] + 0.5;
        }

        void mapInverse(int x, int y, int &newX, int &newY) {
            // multiply vector [x, y, 1] by the matrix
            if (inverseDirty) {
                inverse = matrix.invert();
                inverseDirty = false;
            }
            newX = x*inverse.m[0][0] + y*inverse.m[1][0] + 1*inverse.m[2][0] + 0.5;
            newY = x*inverse.m[0][1] + y*inverse.m[1][1] + 1*inverse.m[2][1] + 0.5;
        }

    };

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

    /* as a rule, bounds-checking in Framebuffer should do nothing on a write,
     * and read color::CLEAR on a get.
     */
    // TODO: draw text
    class Framebuffer {
    public:
        pixel *pixels = nullptr;
        int width = 0, height = 0;

        Framebuffer(int width, int height) {
            if(width <= 0 || height <= 0)
                throw std::domain_error("framebuffer width/height must be positive");
            this->width = width;
            this->height = height;
            pixels = new pixel[width * height];
        }

        Framebuffer(std::string filename) {
            png_image image = { 0 }; /* The control structure used by libpng */

            image.version = PNG_IMAGE_VERSION;

            if (!png_image_begin_read_from_file(&image, filename.c_str()))
                throw std::runtime_error("libpng image_begin_read_from_file failed");
            image.format = PNG_FORMAT_RGBA;
            width = image.width;
            height = image.height;
            pixels = new pixel[width * height];
            if (!png_image_finish_read(&image, NULL/*background*/, pixels,
                                       0/*row_stride*/, NULL/*colormap*/)) {
                png_image_free(&image);
                delete pixels;
                throw std::runtime_error("libpng image_finish_read failed");
            }
        }

        void save(std::string filename) {
            png_image image = { 0 }; /* The control structure used by libpng */

            image.version = PNG_IMAGE_VERSION;
            image.format = PNG_FORMAT_RGBA;
            image.width = width;
            image.height = height;

            if (!png_image_write_to_file(&image, filename.c_str(), 0/*convert_to_8bit*/,
                                         pixels, 0/*row_stride*/, NULL/*colormap*/))
                throw std::runtime_error("libpng image_write_to_file failed");
        }

        ~Framebuffer() {
            if (pixels)
                delete pixels;
        }

        pixel& _at(int x, int y) {
            return pixels[x + y*width];
        }
        pixel& at(int x, int y) {
            if (x < 0 || x >= width || y < 0 || y >= height)
                throw std::out_of_range("Invalid x/y");
            return _at(x, y);
        }

        pixel get(int x, int y) {
            if (x < 0 || x >= width || y < 0 || y >= height)
                return color::CLEAR;
            return _at(x, y);
        }

        template<typename... Targs>
        void set(int x, int y, Targs... Fargs) {
            if (x < 0 || x >= width || y < 0 || y >= height)
                return;
            _at(x, y).set(Fargs...);
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
                            _at(x, i).set(p);
                        if (x + w == endX)
                            _at(x + w - 1, i).set(p);
                    }
        }

        void lineDraw(int x, int y, int w, int h, pixel p) {
            float slope = (float)h / w;

            if (slope > 1 || slope < -1) {
                if (h < 0) {
                    w = -w;
                    x -= w;
                    h = -h;
                    y -= h;
                }
                slope = (float)w / h;
                for (int j=0; j<h; ++j) {
                    if ((y + j) < 0 || (y + j) >= height)
                        continue;
                    int i = slope * j;
                    if ((x + i) < 0 || (x + i) >= width)
                        continue;

                    set(x+i,y+j, p);
                }
            }
            else {
                if (w < 0) {
                    w = -w;
                    x -= w;
                    h = -h;
                    y -= h;
                }
                for (int i=0; i<w; ++i) {
                    if ((x + i) < 0 || (x + i) >= width)
                        continue;
                    int j = slope * i;
                    if ((y + j) < 0 || (y + j) >= height)
                        continue;

                    set(x+i,y+j, p);
                }
            }
        }

        //TODO: this works very poorly. In fact, it barely works at all.
        void arcFill(int x, int y, int r, float start, float end, pixel p) {
            int sx = floor(0.5 + (r * cos(start))) + x;
            int sy = floor(0.5 + (r * sin(start))) + y;
            int ex = floor(0.5 + (r * cos(end))) + x;
            int ey = floor(0.5 + (r * sin(end))) + y;
            using std::abs;
            
            do {
                lineDraw(x, y, sx - x, sy - y, p);

                int dx = sx - x, dy = sy - y;
                
                if (abs(dy) > abs(dx) && dy < abs(dx)) {
                    //north side
                    sx--; sy = -sqrtf(r * r - (sx - x) * (sx - x)) + y;
                }
                else if (abs(dy) <= abs(dx) && dx > abs(dy)) {
                    // east side
                    sy--; sx = sqrtf(r * r - (sy - y) * (sy - y)) + x;
                }
                else if (abs(dy) > abs(dx) && dy > -abs(dx)) {
                    // south side
                    sx++; sy = sqrtf(r * r - (sx - x) * (sx - x)) + y;
                }
                else {
                    // west side
                    sy++; sx = -sqrtf(r * r - (sy - y) * (sy - y)) + x;
                }
            } while(sx != ex || sy != ey); 
        }

        void arcDraw(int x, int y, int r, float start, float end, pixel p) {
            int sx = floor(0.5 + (r * cos(start))) + x;
            int sy = floor(0.5 + (r * sin(start))) + y;
            int ex = floor(0.5 + (r * cos(end))) + x;
            int ey = floor(0.5 + (r * sin(end))) + y;
            using std::abs;
            
            do {
                if(sx > 0 && sy > 0 && sx < width && sy < height)
                    set(sx, sy, p);

                int dx = sx - x, dy = sy - y;
                
                if (abs(dy) > abs(dx) && dy < abs(dx)) {
                    //north side
                    sx--; sy = -sqrtf(r * r - (sx - x) * (sx - x)) + y;
                }
                else if (abs(dy) <= abs(dx) && dx > abs(dy)) {
                    // east side
                    sy--; sx = sqrtf(r * r - (sy - y) * (sy - y)) + x;
                }
                else if (abs(dy) > abs(dx) && dy > -abs(dx)) {
                    // south side
                    sx++; sy = sqrtf(r * r - (sx - x) * (sx - x)) + y;
                }
                else {
                    // west side
                    sy++; sx = -sqrtf(r * r - (sy - y) * (sy - y)) + x;
                }
            } while(sx != ex || sy != ey); 
        }

        void blit(Framebuffer *from, int x, int y) {
            blit(from, 0, 0, from->width, from->height, x, y);
        }

        void blit(Framebuffer *from, int fromX, int fromY, int w, int h, int x, int y) {
            int startX = x, startY = y;
            if (_clipRect(x, y, w, h)) {
                int ox = x - startX + fromX, oy = y - startY + fromY;
                for (int i = y; i < y+h; ++i) {
                    for (int j = x; j < x+w; ++j) {
                        pixel fp = from->get(j + ox, i + oy);
                        if (fp.a == 0xff)
                            at(j, i).set(fp);
                        else if (fp.a > 0) {
                            auto &tp = at(j, i);
                            float mix = fp.a / 255.0f;
                            // TODO: blend things
                            //tp.set(, 0, 0, 0xff);
                        }
                    }
                }
            }
        }

        void blit(Framebuffer *from, Affine transform) {
            blit(from, 0, 0, from->width, from->height, transform);
        }
        //TODO: *maybe* an optional "to" location to allow reusing transforms more easily?
        void blit(Framebuffer *from, int fromX, int fromY, int fromW, int fromH, Affine transform) {
            // generate bounding box
            int x1, y1, x2, y2;
            {
                int tx, ty;
                transform.map(fromX, fromY, x1, y1);
                x2 = x1; y2 = y1;
                transform.map(fromX + fromW, fromY, tx, ty);
                x1 = std::min(x1, tx); x2 = std::max(x2, tx);
                y1 = std::min(y1, ty); y2 = std::max(y2, ty);
                transform.map(fromX + fromW, fromY + fromH, tx, ty);
                x1 = std::min(x1, tx); x2 = std::max(x2, tx);
                y1 = std::min(y1, ty); y2 = std::max(y2, ty);
                transform.map(fromX, fromY + fromH, tx, ty);
                x1 = std::min(x1, tx); x2 = std::max(x2, tx);
                y1 = std::min(y1, ty); y2 = std::max(y2, ty);
            }
            int w = x2 - x1;
            int h = y2 - y1;

            // do the blit
            if(_clipRect(x1, y1, w, h))
                for (int j=y1; j<y1+h; ++j)
                    for (int i=x1; i<x1+w; ++i)
                    {
                        int x, y;
                        transform.mapInverse(i, j, x, y);
                        if (x > fromX && y > fromY && x < fromX + fromW && y < fromY + fromH) {
                            auto &fp = from->_at(x, y);
                            if (fp.a == 0xff)
                                _at(i, j).set(fp);
                            else if (fp.a > 0) {
                                auto &tp = _at(i, j);
                                float mix = fp.a / 255.0f;
                                // TODO: blend things
                                //tp.set(, 0, 0, 0xff);
                            }
                        }
                    }
        }
    };

    //TODO: gl and glfw exceptions
    //TODO: fullscreen?
    //TODO: support replacing frame after startup / changing frame size
    //TODO: mouse callbacks
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

        static void rawResizeCallback(GLFWwindow *window, int width, int height) {
            FramebufferWindow *me = (FramebufferWindow*)glfwGetWindowUserPointer(window);
            me->resizeEvent(width, height);
            me->needsResize = true;
        }

        virtual void resizeEvent(int width, int height) { }
        virtual void keyboardEvent(int key, int scancode, int action, int mods) { }

        void mainLoop() {
            if (!frame)
                throw std::domain_error("Must set framebuffer before starting window");

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

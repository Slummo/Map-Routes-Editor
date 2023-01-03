
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <utility>
#include <mutex>
#include <glad/glad.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <optional>
#include <napi.h>
#include "Framebuffer.hpp"
#include "Event.hpp"
#include "Program.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "VertexBuffer.hpp"
#include "VertexArray.hpp"

typedef void *LPVOID;
typedef uint8_t BOOL;
typedef BOOL *LPBOOL;
typedef void (*LPTHREADROUTINE)(LPVOID, LPBOOL);
typedef struct {
    size_t& width;
    size_t& height;
    GLEventQueue& queue;
} GLThreadArgs;

typedef std::pair<uint32_t, uint32_t> Vec2u;

#define CheckGLError() _CheckGLError(__FILE__, __LINE__)

void _CheckGLError(const char* file, int line);

GLFWwindow* gl_renderWndw(const char* name, Vec2u size);

class Color {
    public:
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;

        Color(uint8_t r, uint8_t b, uint8_t g, uint8_t a) : r(r), g(g), b(b), a(a) {};
        Color(const Color& other) : r(other.r), g(other.g), b(other.b), a(other.a) {}; 

        Color() : r(0), g(0), b(0), a(0) {};
};

const Color WHITE = Color(1, 1, 1, 1);
const Color BLACK = Color(0, 0, 0, 1);

void clearColor(Color color);

class OpenGLThread : public Napi::AsyncWorker {
    public:
        OpenGLThread(Napi::Env env, LPTHREADROUTINE OGLfunction, GLThreadArgs args) : AsyncWorker(env), routine(OGLfunction), args(args) {};

        ~OpenGLThread() {};

        void Stop() {
            keepExecuting = 0;
        }

    private: 
        LPTHREADROUTINE routine;
        GLThreadArgs args;
        BOOL keepExecuting = 1;

        void Execute() override {
            routine((LPVOID)&args, &keepExecuting);
        };

        void OnOK() override {
        }
};

#endif
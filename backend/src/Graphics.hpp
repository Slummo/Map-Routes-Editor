
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
#include "Buffer.hpp"
#include "Renderer.hpp"
#include "VertexArray.hpp"
#include "Color.hpp"

typedef void *LPVOID;
typedef uint8_t BOOL;
typedef BOOL *LPBOOL;
typedef void (*LPTHREADROUTINE)(LPVOID, LPBOOL);
typedef struct {
    size_t& width;
    size_t& height;
    size_t& cameraX;
    size_t& cameraZ;
    float& camTheta;
    uint8_t*& buffer;
    std::string& mapPath;
    GLEventQueue& queue;
} GLThreadArgs;

typedef std::pair<uint32_t, uint32_t> Vec2u;

GLFWwindow* gl_renderWndw(const char* name, Vec2u size);

enum class RenderTarget {
    STARTUP_ANIMATION = 0,
    IDLE_MENU,
    MAP, 
};

void clearColor(Color color);
VertexArray createGrid(float gridSize, int tiles);
VertexArray createQuad(float quadSize);

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
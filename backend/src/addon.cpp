
#include <iostream>
#include <fstream>
#include <node.h>
#include <napi.h>
#include <condition_variable>
#include <chrono>
#include "Graphics.hpp"

#define BYTES_PER_PIXEL 4

size_t screenWidth, screenHeight;

OpenGLThread* GLThread;
GLEventQueue queue;

uint8_t* buffer;

std::mutex mutex;
std::condition_variable cond;
bool eventHandled = false;

void OpenGLEngine(LPVOID args, LPBOOL keepExecuting) {
  GLThreadArgs* fArgs = (GLThreadArgs*)args;

  Vec2u dimentions(1, 1);

  GLFWwindow* window = gl_renderWndw("master", dimentions);
  glfwHideWindow(window);
  glViewport(0, 0, (fArgs->width), (fArgs->height));
  glfwSwapInterval(1);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  Framebuffer FBO(dimentions);

  GLfloat vertices[] = {
      0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
      0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
      -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
      0.0f, 0.0f, 0.0f, 0.5f, 0.5f
  };

  GLuint indices[] = {
      0, 1, 4,
      0, 3, 4,
      3, 4, 2,
      2, 4, 1
  };

  Texture pTexture("frontend/docs/2ddu.jpg", GL_TEXTURE_2D);
  pTexture.bind();

  VertexArray VAO;
  VAO.bind();

  GLuint elementBuffer;

  VertexBuffer VBO((uint8_t*)vertices, sizeof(vertices));

  glGenBuffers(1, &elementBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  VBO.setAttribPointer(0, 3, GL_FLOAT, 5 * sizeof(float), NULL);
  VBO.setAttribPointer(1, 2, GL_FLOAT, 5 * sizeof(float), 3 * sizeof(float));

  VBO.bind();

  Program pProgram("backend/shaders/vertexShader.glsl", "backend/shaders/fragmentShader.glsl");
  pProgram.bind();
  pTexture.bind();

  Camera cam(fArgs->width, fArgs->height, 1, 100, glm::vec3(0, 0, 2), glm::vec3(0, 0, -1));

  glm::mat4 rot(1.0f);
  rot = glm::rotate(rot, glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));
  GLuint gMat = pProgram.getUniformLocaiton("transform");
  GLuint cMat = pProgram.getUniformLocaiton("camMatrix");
  float deg = 1;
  
  while ((*keepExecuting)) {
    while (queue.size() > 0) {
      switch (queue.pop())
      {
        case GLEvent::REQUEST_FRAME: {
          cam.updateMatrices();
          glm::mat4 vProj = cam.getViewProjection();
          
          rot = glm::rotate(rot, glm::radians(deg), glm::vec3(0.0f, 1.0f, 0.0f));

          glUniformMatrix4fv(gMat, 1, GL_FALSE, glm::value_ptr(rot));
          glUniformMatrix4fv(cMat, 1, GL_FALSE, glm::value_ptr(vProj));
          
          FBO.bind();

          glClearColor((GLfloat) 128/255, (GLfloat) 58/255, (GLfloat) 153/255, 1);
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          
          glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(GLfloat), GL_UNSIGNED_INT, 0);
          FBO.readData(buffer, BYTES_PER_PIXEL);
        
          {
            std::lock_guard lock(mutex);
            eventHandled = true;
            cond.notify_one();
          }
          
          FBO.unbind();
          glClearColor((GLfloat) 60/255, (GLfloat) 24/255, (GLfloat) 72/255, 1);
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

          glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(GLfloat), GL_UNSIGNED_INT, 0);

          glfwSwapBuffers(window);
          glfwPollEvents();

          break;
        }
        case GLEvent::RESIZE_FRAME:
          cam.setSize(fArgs->width, fArgs->height);
          FBO.changeDimentions(fArgs->width, fArgs->height);
          break;
      }
    }
  }

  glfwTerminate();
}

Napi::Value startEngine(const Napi::CallbackInfo& info) {
  if (info.Length() < 2 && (!info[0].IsNumber() | !info[1].IsNumber())) {
    return info.Env().Undefined();
  }
  
  screenWidth = (size_t) info[0].As<Napi::Number>().Uint32Value();
  screenHeight = (size_t) info[1].As<Napi::Number>().Uint32Value();

  GLThreadArgs args{screenWidth, screenHeight, queue};
  
  GLThread = new OpenGLThread(info.Env(), OpenGLEngine, args);

  GLThread->Queue();

  return info.Env().Undefined();
}

Napi::Value stopEngine(const Napi::CallbackInfo& info) {
  GLThread->Stop();

  return info.Env().Undefined();
}

Napi::ArrayBuffer loadFrame(const Napi::CallbackInfo& info) {
  Napi::ArrayBuffer frameBuffer = Napi::ArrayBuffer::New(info.Env(), screenWidth * screenHeight * BYTES_PER_PIXEL * sizeof(uint8_t));

  buffer = (uint8_t*)frameBuffer.Data();

  queue.push(GLEvent::REQUEST_FRAME);
  
  {
  std::unique_lock lock(mutex);
  cond.wait(lock, []{ return eventHandled; });
  eventHandled = false;
  }

  return frameBuffer;
}

Napi::Value resizeFramebuffer(const Napi::CallbackInfo& info) {
  if (info.Length() < 2 || !info[0].IsNumber() || !info[1].IsNumber()) {
    return info.Env().Undefined();
  }

  screenWidth = (size_t) info[0].As<Napi::Number>().Uint32Value();
  screenHeight = (size_t) info[1].As<Napi::Number>().Uint32Value();
  queue.push(GLEvent::RESIZE_FRAME);

  return info.Env().Undefined();
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  exports.Set("startEngine", Napi::Function::New(env, startEngine));
  exports.Set("stopEngine", Napi::Function::New(env, stopEngine));
  exports.Set("loadFrame", Napi::Function::New(env, loadFrame));
  exports.Set("resizeFrame", Napi::Function::New(env, resizeFramebuffer));

  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll);

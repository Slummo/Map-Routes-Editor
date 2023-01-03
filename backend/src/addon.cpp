
#include <iostream>
#include <fstream>
#include <napi.h>
#include <condition_variable>
#include <chrono>
#include <numbers>
#include "Graphics.hpp"

#define BYTES_PER_PIXEL 4
#define SAMPLES 8
const double PI = 3.141592653589793238463;


size_t screenWidth, screenHeight;

OpenGLThread* GLThread;
GLEventQueue queue;

uint8_t* buffer;

std::mutex mutex;
std::condition_variable cond;
bool eventHandled = false;

void OpenGLEngine(LPVOID args, LPBOOL keepExecuting) {
  GLThreadArgs* fArgs = (GLThreadArgs*)args;

  Vec2u dimentions((fArgs->width), (fArgs->height));
  
  GLFWwindow* window = gl_renderWndw("master", dimentions);
  //glfwHideWindow(window);
  glViewport(0, 0, (fArgs->width), (fArgs->height));
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_TEXTURE_2D); 
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  Framebuffer FBO((fArgs->width), (fArgs->height));

  std::vector<GLfloat> quadVertices{
    -1.f,  0.f, -1.f, 
    -1.f,  0.f,  1.f, 
    1.f, 0.f,  1.f, 

    1.f,  0.f, -1.f, 
    1.f,  0.f,  1.f, 
    -1.f,  0.f, -1.f, 
  };

  Program quadProgram("backend/shaders/quadVert.glsl", "backend/shaders/quadFrag.glsl");

  VertexArray quadVAO;
  quadVAO.bind();

  VertexBuffer quadVBO((uint8_t*) quadVertices.data(), quadVertices.size() * sizeof(GLfloat));
  quadVBO.bind();

  quadVBO.setAttribPointer(0, 3, GL_FLOAT, 3 * sizeof(GLfloat), NULL);

  quadVAO.unbind();
  quadVBO.unbind();

  // Setting up the grid

  glLineWidth(2.f);

  const int slices = 1000;
  const float gridSize = 1000;

  std::vector<glm::vec3> vertices;
  std::vector<glm::uvec2> indices;
  
  for (int i = -(slices/2); i < (slices/2 + 1); i++) {
    for (int j = -(slices/2); j < (slices/2 + 1); j++) {
      vertices.push_back(glm::vec3(((float) gridSize / slices) * j, 0, ((float) gridSize / slices) * i));
    }
  }

  for (int i = 0; i < slices + 1; i++) {
    indices.push_back(glm::uvec2((slices + 1) * i, (slices+1) * i + slices));
    indices.push_back(glm::uvec2(i, (slices + 1) * slices + i));
  }

  Program gridProgram("backend/shaders/gridVert.glsl", "backend/shaders/gridFrag.glsl");

  VertexArray gridVAO;
  gridVAO.bind();

  VertexBuffer gridVBO((uint8_t*)glm::value_ptr(vertices[0]), vertices.size() * sizeof(glm::vec3));
  gridVBO.bind();

  gridVBO.setAttribPointer(0, 3, GL_FLOAT, 3 * sizeof(float), NULL);

  GLuint gridEBO;
  glGenBuffers(1, &gridEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(glm::uvec2), glm::value_ptr(indices[0]), GL_STATIC_DRAW);

  gridVAO.unbind();
  gridVBO.unbind();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // End of grid setup

  auto orientation = glm::normalize(glm::rotate(glm::mat4(1.0f), glm::radians(-30.0f), glm::vec3(1, 0, 0)) * glm::vec4(0, 0, -1, 0));
  Camera cam(fArgs->width, fArgs->height, 1, 100, glm::vec3(1, 5, 1), glm::vec3(0, 0, 0));

  GLuint quadCameraMatrix = quadProgram.getUniformLocaiton("camMatrix");
  GLuint quadResUniform = quadProgram.getUniformLocaiton("resolution");

  GLuint gridCameraMatrix = gridProgram.getUniformLocaiton("camMatrix");
  GLuint gridCameraPosition = gridProgram.getUniformLocaiton("camPos");
  
  while ((*keepExecuting)) {
    while (queue.size() > 0) {
      switch (queue.pop())
      {
        case GLEvent::REQUEST_FRAME: {

          // Camera stuff
          cam.setPosition(glm::vec3(5 * sin(glfwGetTime()), 5, 5 * cos(glfwGetTime())));
          cam.updateMatrices();
          glm::mat4 camViewProj = cam.getViewProjection();

          FBO.bind();

          clearColor(BLACK);
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          
          // Render quad
          quadVAO.bind();
          quadProgram.bind();

          // Set up uniforms
          glUniformMatrix4fv(quadCameraMatrix, 1, GL_FALSE, glm::value_ptr(camViewProj));
          glUniform2f(quadResUniform, fArgs->width, fArgs->height);

          glDrawArrays(GL_TRIANGLES, 0, 6);

          quadVAO.unbind();
          quadProgram.unbind();
          
          // Render grid
          gridVAO.bind();
          gridProgram.bind();

          // Set up uniforms
          glUniformMatrix4fv(gridCameraMatrix, 1, GL_FALSE, glm::value_ptr(camViewProj));
          glUniform3fv(gridCameraPosition, 1, glm::value_ptr(cam.getPosition()));

          glDisable(GL_DEPTH_TEST);

          glDrawElements(GL_LINES, (GLuint)indices.size() * 4, GL_UNSIGNED_INT, NULL);

          glEnable(GL_DEPTH_TEST);

          gridVAO.unbind();
          gridProgram.unbind();

          FBO.readData(buffer, BYTES_PER_PIXEL);
          FBO.unbind();

          {
            std::lock_guard lock(mutex);
            eventHandled = true;
            cond.notify_one();
          }

          clearColor(BLACK);
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

          // Render quad
          quadVAO.bind();
          quadProgram.bind();

          // Set up uniforms
          glUniformMatrix4fv(quadCameraMatrix, 1, GL_FALSE, glm::value_ptr(camViewProj));
          glUniform2f(quadResUniform, fArgs->width, fArgs->height);

          glDrawArrays(GL_TRIANGLES, 0, 6);

          quadVAO.unbind();
          quadProgram.unbind();
          

          // Render grid
          gridVAO.bind();
          gridProgram.bind();

          // Set up uniforms
          glUniformMatrix4fv(gridCameraMatrix, 1, GL_FALSE, glm::value_ptr(camViewProj));
          glUniform3fv(gridCameraPosition, 1, glm::value_ptr(cam.getPosition()));

          glDisable(GL_DEPTH_TEST);

          glDrawElements(GL_LINES, (GLuint)indices.size() * 4, GL_UNSIGNED_INT, NULL);

          glEnable(GL_DEPTH_TEST);

          gridVAO.unbind();
          gridProgram.unbind();

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

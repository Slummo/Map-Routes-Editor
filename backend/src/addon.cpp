
#include <iostream>
#include <fstream>
#include <condition_variable>
#include <chrono>
#include <cmath>
#include "Graphics.hpp"

#define BYTES_PER_PIXEL 4
#define SAMPLES 8
const double PI = 3.141592653589793238463;

size_t screenWidth, screenHeight;
size_t cameraX, cameraZ;
float camTheta;

OpenGLThread* GLThread;
GLEventQueue queue;

uint8_t* buffer;

std::mutex mutex;
std::condition_variable cond;
bool eventHandled = false;

std::string currentMapPath;
RenderTarget currentRT;

void OpenGLEngine(LPVOID args, LPBOOL keepExecuting) {
  GLThreadArgs* fArgs = (GLThreadArgs*)args;

  Vec2u dimentions((fArgs->width), (fArgs->height));
  
  GLFWwindow* window = gl_renderWndw("master", dimentions);
  glfwHideWindow(window);
  glViewport(0, 0, (fArgs->width), (fArgs->height));
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  
  Framebuffer FBO((fArgs->width), (fArgs->height));

  Program quadProgram("backend/shaders/quadVert.glsl", "backend/shaders/quadFrag.glsl");
  Program animationQuadProgram("backend/shaders/quadVert.glsl", "backend/shaders/animationQuadFrag.glsl");
  VertexArray quadVAO = createQuad(1);
  // Setting up the grid

  glLineWidth(2.f);

  Program gridProgram("backend/shaders/gridVert.glsl", "backend/shaders/gridFrag.glsl");
  Program animationGridProgram("backend/shaders/gridVert.glsl", "backend/shaders/animationGridFrag.glsl");
  VertexArray gridVAO = createGrid(100, 100);

  // End of grid setup

  auto orientation = glm::normalize(glm::rotate(glm::mat4(1.0f), glm::radians(-30.0f), glm::vec3(1, 0, 0)) * glm::vec4(0, 0, -1, 0));
  Camera cam(fArgs->width, fArgs->height, 1, 100, glm::vec3(5, 5, 0), glm::vec3(0.f));

  GLuint quadCameraMatrix = quadProgram.getUniformLocaiton("camMatrix");
  GLuint quadResUniform = quadProgram.getUniformLocaiton("resolution");

  GLuint gridCameraMatrix = gridProgram.getUniformLocaiton("camMatrix");
  GLuint gridCameraPosition = gridProgram.getUniformLocaiton("camPos");

  GLuint line = 45;
  GLuint animationFrame = 0;

  Texture* Map = nullptr;
  VertexBuffer* mapVertices = nullptr;
  
  while (queue.size() > 0 || (*keepExecuting)) {

    switch (queue.pop())
    {
      case GLEvent::REQUEST_FRAME: {

        switch (currentRT) {
          case RenderTarget::STARTUP_ANIMATION: {
              cam.updateMatrices();
              glm::mat4 camViewProj = cam.getViewProjection();

              FBO.bind();

              clearColor(BLACK);
              glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

              if (line > 160) {
                quadVAO.bind();

                animationQuadProgram.bind();
                GLuint camMatrixQuad = animationQuadProgram.getUniformLocaiton("camMatrix");
                GLuint timeQuad = animationQuadProgram.getUniformLocaiton("time");
                GLuint resQuad = animationQuadProgram.getUniformLocaiton("resolution");
                glUniformMatrix4fv(camMatrixQuad, 1, GL_FALSE, glm::value_ptr(camViewProj));
                glUniform1f(timeQuad, line);
                glUniform2f(resQuad, fArgs->width, fArgs->height);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                animationQuadProgram.unbind();
                
                line++;
              }

              gridVAO.bind();

              glDisable(GL_DEPTH_TEST);

              gridProgram.bind();
              glUniformMatrix4fv(gridCameraMatrix, 1, GL_FALSE, glm::value_ptr(camViewProj));
              glUniform3fv(gridCameraPosition, 1, glm::value_ptr(cam.getPosition()));
              glDrawElements(GL_LINES, 2 * (line - 1), GL_UNSIGNED_INT, NULL);
              gridProgram.unbind();
              
              animationGridProgram.bind();
              GLuint camMatrix = animationGridProgram.getUniformLocaiton("camMatrix");
              GLuint time = animationGridProgram.getUniformLocaiton("time");
              glUniformMatrix4fv(camMatrix, 1, GL_FALSE, glm::value_ptr(camViewProj));
              glUniform1f(time, animationFrame);
              glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(2 * (line - 1) * sizeof(GLuint)));
              animationGridProgram.unbind();

              glEnable(GL_DEPTH_TEST);

              gridVAO.unbind();

              FBO.readData(fArgs->buffer, BYTES_PER_PIXEL);

              {
              std::lock_guard lock(mutex);
              eventHandled = true;
              cond.notify_one();
              }

              FBO.unbind();

              animationFrame++;
              if (animationFrame == 30 && line < 162) {line++; animationFrame = 0;}
              if (line == 60) line = 143;
              if (line == 560) currentRT = RenderTarget::IDLE_MENU;
            break;
          }
          case RenderTarget::IDLE_MENU: {
            // Camera stuff
            cam.setPosition(glm::vec3(glm::rotateY(glm::vec4(cam.getPosition(), 1), glm::radians(0.5f))));
            cam.updateMatrices();
            glm::mat4 camViewProj = cam.getViewProjection();

            FBO.bind();

            clearColor(BLACK);
            //glClearColor(1, 1, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
              
            // Render quad
            quadVAO.bind();
            quadProgram.bind();

            // Set up uniforms
            glUniformMatrix4fv(quadCameraMatrix, 1, GL_FALSE, glm::value_ptr(camViewProj));
            glUniform2f(quadResUniform, fArgs->width, fArgs->height);

            glDrawArrays(GL_TRIANGLES, 0, 6);
              
            // Render grid
            gridVAO.bind();
            gridProgram.bind();

            // Set up uniforms
            glUniformMatrix4fv(gridCameraMatrix, 1, GL_FALSE, glm::value_ptr(camViewProj));
            glUniform3fv(gridCameraPosition, 1, glm::value_ptr(cam.getPosition()));

            glDisable(GL_DEPTH_TEST);

            glDrawElements(GL_LINES, gridVAO.getIndexBuffer()->getCount(), GL_UNSIGNED_INT, NULL);

            glEnable(GL_DEPTH_TEST);

            FBO.readData(buffer, BYTES_PER_PIXEL);
            FBO.unbind();

            {
              std::lock_guard lock(mutex);
              eventHandled = true;
              cond.notify_one();
            }

          break;
          }
          case RenderTarget::MAP: {
            cam.updateMatrices();
            glm::mat4 camViewProj = cam.getViewProjection();

            
          }
          break;
        }
        // glfwSwapBuffers(window);
        // glfwPollEvents();
        break;
      }

      case GLEvent::RESIZE_FRAME: {
        cam.setSize(fArgs->width, fArgs->height);
        FBO.changeDimentions(fArgs->width, fArgs->height);
      break;
      }
      case GLEvent::MOVE_CAMERA: {
        if (isnan(fArgs->camTheta)) { fArgs->camTheta = 0.0f;}
        cam.setPosition(glm::vec3(glm::rotateY(glm::vec4(cam.getPosition(), 1), fArgs->camTheta / 10)));
      break;
      }
      case GLEvent::LOAD_MAP: {
        Map = new Texture(fArgs->mapPath.c_str(), GL_TEXTURE_2D);

        currentRT = RenderTarget::MAP;
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

  GLThreadArgs args{screenWidth, screenHeight, cameraX, cameraZ, camTheta, buffer, currentMapPath, queue};
  
  GLThread = new OpenGLThread(info.Env(), OpenGLEngine, args);

  currentRT = RenderTarget::STARTUP_ANIMATION;

  GLThread->Queue();

  return info.Env().Undefined();
}

Napi::Value stopEngine(const Napi::CallbackInfo& info) {
  GLThread->Stop();

  return info.Env().Undefined();
}

Napi::ArrayBuffer loadFrame(const Napi::CallbackInfo& info) {
  Napi::ArrayBuffer frameBuffer = Napi::ArrayBuffer::New(info.Env(), screenWidth * screenHeight * BYTES_PER_PIXEL * sizeof(uint8_t));

  buffer = (uint8_t*) frameBuffer.Data();

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

Napi::Value moveCamera(const Napi::CallbackInfo& info) {
  if (info.Length() < 2 || !info[0].IsNumber() || !info[1].IsNumber()) {
    return info.Env().Undefined();
  }

  cameraX = (size_t) info[0].As<Napi::Number>().Uint32Value();
  cameraZ = (size_t) info[1].As<Napi::Number>().Uint32Value();
  camTheta = (float) info[2].As<Napi::Number>().FloatValue();
  queue.push(GLEvent::MOVE_CAMERA);

  return info.Env().Undefined();
}

Napi::Value loadMap(const Napi::CallbackInfo& info) {
  currentMapPath = info[0].As<Napi::String>().Utf8Value().c_str();

  queue.push(GLEvent::LOAD_MAP);

  return info.Env().Undefined();
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  exports.Set("startEngine", Napi::Function::New(env, startEngine));
  exports.Set("stopEngine", Napi::Function::New(env, stopEngine));
  exports.Set("loadFrame", Napi::Function::New(env, loadFrame));
  exports.Set("resizeFrame", Napi::Function::New(env, resizeFramebuffer));
  exports.Set("moveCamera", Napi::Function::New(env, moveCamera));

  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll);

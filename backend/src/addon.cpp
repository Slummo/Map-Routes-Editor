#include <iostream>
#include <fstream>
#include <node.h>
#include <napi.h>
#include "Graphics.h"

#define BYTES_PER_PIXEL 4

typedef void *LPVOID;
OpenGLThread* GLThread;
GLuint screenWidth, screenHeight;
GLFWwindow* renderWindow;
uint8_t* renderBuffer;

void OpenGLEngine(LPVOID args, LPBOOL keepExecuting) {
  GLThreadArgs* fArgs = (GLThreadArgs*)args;

  Vec2u dimentions(fArgs->width, fArgs->height);

  *(fArgs->window) = gl_renderWndw("master", dimentions);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);

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

  GLuint vertexBuffer;
  GLuint VAO;
  GLuint elementBuffer;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); 
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glGenBuffers(1, &elementBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
  glEnableVertexAttribArray(1);

  GLuint FBO;
  glGenFramebuffers(1, &FBO);

  GLuint textureAttachment;
  glGenTextures(1, &textureAttachment);
  glBindTexture(GL_TEXTURE_2D, textureAttachment);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dimentions.first, dimentions.second, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  GLuint RBO;
  glGenRenderbuffers(1, &RBO);
  glBindRenderbuffer(GL_RENDERBUFFER, RBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, dimentions.first, dimentions.second);

  glBindFramebuffer(GL_FRAMEBUFFER, FBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureAttachment, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

  Program pProgram("backend/shaders/vertexShader.glsl", "backend/shaders/fragmentShader.glsl");
  pProgram.bind();
  pTexture.bind();

  glm::mat4 rot(1.0f);
  rot = glm::rotate(rot, glm::radians(90.f), glm::vec3(0.0f, 0.0f, 1.0f));
  GLuint gMat = pProgram.getUniformLocaiton("transform");
  float deg = 0.5;
  
  while (!glfwWindowShouldClose(*(fArgs->window)) && (*keepExecuting)) {
    rot = glm::rotate(rot, glm::radians(deg), glm::vec3(1.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(gMat, 1, GL_FALSE, glm::value_ptr(rot));

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearColor(0.25, 0.5, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(GLfloat), GL_UNSIGNED_INT, 0);
    glReadPixels(0, 0, dimentions.first, dimentions.second, BYTES_PER_PIXEL == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, fArgs->buffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.25, 0.5, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(GLfloat), GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(*(fArgs->window));
    glfwPollEvents();
  }

  glfwTerminate();
}

Napi::Value startEngine(const Napi::CallbackInfo& info) {
  if (info.Length() < 2 && (!info[0].IsNumber() | !info[1].IsNumber())) {
    return info.Env().Undefined();
  }
  
  screenWidth = info[0].As<Napi::Number>().Uint32Value();
  screenHeight = info[1].As<Napi::Number>().Uint32Value();
  renderBuffer = new uint8_t[screenWidth * screenHeight * BYTES_PER_PIXEL];
  
  GLThreadArgs args{screenWidth, screenHeight, &renderWindow, renderBuffer};

  GLThread = new OpenGLThread(info.Env(), OpenGLEngine, args);

  GLThread->Queue();

  return info.Env().Undefined();
}

Napi::Value stopEngine(const Napi::CallbackInfo& info) {
  GLThread->Stop();

  return info.Env().Undefined();
}

Napi::ArrayBuffer loadFrame(const Napi::CallbackInfo& info) {
  Napi::ArrayBuffer frameBuffer = Napi::ArrayBuffer::New(info.Env(), screenWidth* screenHeight * BYTES_PER_PIXEL * sizeof(uint8_t));

  std::memcpy(frameBuffer.Data(), renderBuffer, screenWidth * screenHeight * BYTES_PER_PIXEL * sizeof(uint8_t));

  return frameBuffer;
}

Napi::Value resizeFramebuffer(const Napi::CallbackInfo& info) {
  if (info.Length() < 2 && (!info[0].IsNumber() | !info[1].IsNumber())) {
    return info.Env().Undefined();
  }

  //frameResizeCallback(renderWindow, info[0].As<Napi::Number>().Uint32Value(), info[1].As<Napi::Number>().Uint32Value());

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

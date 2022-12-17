#include <iostream>
#include <fstream>
#include <node.h>
#include <napi.h>
#include "Graphics.h"

typedef void *LPVOID;
OpenGLThread* GLThread;
uint8_t* renderBuffer;
GLuint screenWidth, screenHeight;

Napi::ArrayBuffer loadImage(const Napi::CallbackInfo& info) {
  std::ifstream image("eddu.jpg", std::ios::binary);
  Napi::ArrayBuffer imageBuffer;
  int bufferSize;

  image.seekg(0, image.end);
  bufferSize = image.tellg();
  image.seekg(0, image.beg);

  char *data = new char[bufferSize];

  image.read(data, bufferSize);

  imageBuffer = Napi::ArrayBuffer::New(info.Env(), bufferSize);

  memcpy(imageBuffer.Data(), data, bufferSize);

  image.close();
  return imageBuffer;
}

void OpenGLEngine(LPVOID args, LPBOOL keepExecuting) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLThreadArgs* fArgs = (GLThreadArgs*)args;

  Vec2u dimentions(fArgs->width, fArgs->height);

  GLFWwindow* window = gl_renderWndw("eddu", dimentions);
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

  // Texture pTexture("eddu.jpg", GL_TEXTURE_2D);
  // pTexture.bind();

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

  Program pProgram("backend/shaders/vertexShader.glsl", "backend/shaders/fragmentShader.glsl");
  pProgram.bind();

  GLuint FBO;
  glGenFramebuffers(1, &FBO);

  GLuint textureAttachment;
  glGenTextures(1, &textureAttachment);
  glBindTexture(GL_TEXTURE_2D, textureAttachment);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dimentions.first, dimentions.second, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  GLuint renderBuffer;
  glGenRenderbuffers(1, &renderBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, dimentions.first, dimentions.second);

  glBindFramebuffer(GL_FRAMEBUFFER, FBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureAttachment, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
  
  while (!glfwWindowShouldClose(window) && (*keepExecuting)) {
    glClearColor(0.25, 0.5, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(GLfloat), GL_UNSIGNED_INT, 0);
    glReadPixels(0, 0, dimentions.first, dimentions.second, GL_RGB, GL_UNSIGNED_BYTE, fArgs->renderBuffer);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}

Napi::Value startEngine(const Napi::CallbackInfo& info) {
  if (info.Length() < 2) {
    return info.Env().Undefined();
  }
  
  screenWidth = info[0].As<Napi::Number>().Uint32Value();
  screenHeight = info[1].As<Napi::Number>().Uint32Value();
  renderBuffer = new uint8_t[screenWidth * screenHeight * 3];

  GLThreadArgs args{screenWidth, screenHeight, renderBuffer};

  GLThread = new OpenGLThread(info.Env(), OpenGLEngine, args);

  GLThread->Queue();

  return info.Env().Undefined();
}

Napi::Value stopEngine(const Napi::CallbackInfo& info) {
  GLThread->Stop();

  return info.Env().Undefined();
}

Napi::ArrayBuffer loadFrame(const Napi::CallbackInfo& info) {
  Napi::ArrayBuffer frameBuffer = Napi::ArrayBuffer::New(info.Env(), screenWidth* screenHeight * 3 * sizeof(uint8_t));

  std::memcpy(frameBuffer.Data(), renderBuffer, screenWidth * screenHeight * 3 * sizeof(uint8_t));

  return frameBuffer;
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  exports.Set("loadImage", Napi::Function::New(env, loadImage));
  exports.Set("startEngine", Napi::Function::New(env, startEngine));
  exports.Set("stopEngine", Napi::Function::New(env, stopEngine));
  exports.Set("loadFrame", Napi::Function::New(env, loadFrame));

  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll);

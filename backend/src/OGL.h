
#ifndef OGL_H
#define OGL_H

#include <node.h>
#include <napi.h>
#include <iostream>

typedef void *LPVOID;
typedef uint8_t BOOL;
typedef BOOL *LPBOOL;
typedef void (*LPTHREADROUTINE)(void*, LPBOOL);
typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t* renderBuffer;
}   GLThreadArgs;

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
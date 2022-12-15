
#include "Graphics.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLFWwindow* gl_renderWndw(const char* name, Vec2u size) {

    GLFWwindow* window = glfwCreateWindow(size.first, size.second, name, NULL, NULL);

    if (window == NULL) {
        printf("\n[ERROR] Error in creating the window, exiting...");
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("\n[ERROR] Error loading the glad library, exiting...");
        exit(EXIT_FAILURE);
    }

    glViewport(0, 0, size.first, size.second);
    glfwSetFramebufferSizeCallback(window, frameResizeCallback);

    return window;
}

void frameResizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void _CheckGLError(const char* file, int line)
{
    GLenum err ( glGetError() );

    while ( err != GL_NO_ERROR )
    {
        std::string error;
        switch ( err )
        {
            case GL_INVALID_OPERATION:  error="INVALID_OPERATION";      break;
            case GL_INVALID_ENUM:       error="INVALID_ENUM";           break;
            case GL_INVALID_VALUE:      error="INVALID_VALUE";          break;
            case GL_OUT_OF_MEMORY:      error="OUT_OF_MEMORY";          break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
        }
        std::cout << "GL_" << error.c_str() << " - " << file << ":" << line << std::endl;
        err = glGetError();
    }
    return;
}

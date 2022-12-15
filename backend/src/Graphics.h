
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <utility>
#include <glad/glad.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Program.h"
#include "stb_image.h"

typedef std::pair<uint32_t, uint32_t> Vec2u;
typedef unsigned int uint;

#define CheckGLError() _CheckGLError(__FILE__, __LINE__)

void _CheckGLError(const char* file, int line);

struct Color {
    unsigned int r;
    unsigned int g;
    unsigned int b;
    unsigned int a;

    Color(uint r, uint b, uint g, uint a) : r(r), g(g), b(b), a(a) {};
    Color(const Color& other) : r(other.r), g(other.g), b(other.b), a(other.a) {};  

    Color() : r(0), g(0), b(0), a(0) {};
};

class VertexArray {

};

class VertexBuffer {

};

class Texture {
    private:
        GLuint m_texture;
        GLenum m_targetTex;
        int width, height, channels;

    public:
        Texture(const char* imagePath, GLenum textureTarget) : m_targetTex(textureTarget) {
            if (textureTarget != GL_TEXTURE_2D) {
                printf("[ERROR] Texture target not implemented yet, exiting...");
                exit(0);
            }

            glGenTextures(1, &m_texture);
            bind();

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
            glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
            glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_set_flip_vertically_on_load(1);
            GLubyte* data = stbi_load(imagePath, &width, &height, &channels, 0);
            if (!data) {
                printf("[ERROR] Texture load failed, exiting...");
                exit(0);
            }

            glTexImage2D(m_targetTex, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(data);

            glBindTexture(m_targetTex, 0);
        };

        void bind() { glBindTexture(m_targetTex, m_texture); };
        void unbind() { glBindTexture(m_targetTex, 0); };
        int const getWidth() { return width; };
        int const getHeight() { return height; };
        int const getChannels() { return channels; };
        
};

GLFWwindow* gl_renderWndw(const char* name, Vec2u size);
void frameResizeCallback(GLFWwindow* window, int width, int height);

#endif
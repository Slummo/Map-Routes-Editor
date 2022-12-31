
#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/glad.h>
#include <iostream>
#include "stb_image.h"

class Texture {
    private:
        GLuint m_texture;
        GLenum m_targetTex;
        int width, height, channels;

    public:
        Texture(const char* imagePath, GLenum textureTarget) : m_targetTex(textureTarget) {
            if (textureTarget != GL_TEXTURE_2D) {
                std::cout << "[ERROR] Texture target not implemented yet, exiting..." << std::endl;
                exit(EXIT_FAILURE);
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
                std::cout << "[ERROR] Texture load failed, exiting..." << std::endl;
                exit(EXIT_FAILURE);
            }

            glTexImage2D(m_targetTex, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(data);

            glBindTexture(m_targetTex, 0);
        };

        inline void bind() { glBindTexture(m_targetTex, m_texture); };
        inline void unbind() { glBindTexture(m_targetTex, 0); };
        inline int const getWidth() { return width; };
        inline int const getHeight() { return height; };
        inline int const getChannels() { return channels; };
        
};

#endif
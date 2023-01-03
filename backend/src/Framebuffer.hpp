
#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <mutex>
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef std::pair<uint32_t, uint32_t> Vec2u;

class Framebuffer {
    protected:
        // Texture attachment object
        GLuint TAO;

        // Render buffer object
        GLuint RBO;

        // OpenGL ID
        GLuint id;

        // Dimentions of the buffer
        size_t width;
        size_t height;

        // METHODS
        void deleteBuffers();
    
    public:
        Framebuffer() {};

        Framebuffer(size_t _width, size_t _height) : width(_width), height(_height) {
            glGenFramebuffers(1, &id);

            // Keep the refernce to the last bound texture to rebind it after the creation of the texture attachment
            GLint boundTex;
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTex);

            glGenTextures(1, &TAO);
            glBindTexture(GL_TEXTURE_2D, TAO);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei) width, (GLsizei) height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, boundTex);

            glGenRenderbuffers(1, &RBO);
            glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei) width, (GLsizei) height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            // Same as for the textures, keep the reference to the last bound framebuffer
            GLint boundFBO;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &boundFBO);

            glBindFramebuffer(GL_FRAMEBUFFER, id);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TAO, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

            glBindFramebuffer(GL_FRAMEBUFFER, boundFBO);

        };

        Framebuffer(GLuint _TAO, GLuint _RBO) : TAO(_TAO), RBO(_RBO) {
            // Same as for the textures, keep the reference to the last bound framebuffer
            GLint boundFBO;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &boundFBO);

            glBindFramebuffer(GL_FRAMEBUFFER, id);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TAO, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

            glBindFramebuffer(GL_FRAMEBUFFER, boundFBO);
        };

        Framebuffer(Vec2u& dimentions) : width((size_t)dimentions.first), height((size_t)dimentions.second) {
            glGenFramebuffers(1, &id);

            // Keep the refernce to the last bound texture to rebind it after the creation of the texture attachment
            GLint boundTex;
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTex);

            glGenTextures(1, &TAO);
            glBindTexture(GL_TEXTURE_2D, TAO);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei) width, (GLsizei) height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, boundTex);

            glGenRenderbuffers(1, &RBO);
            glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei) width, (GLsizei) height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            // Same as for the textures, keep the reference to the last bound framebuffer
            GLint boundFBO;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &boundFBO);

            glBindFramebuffer(GL_FRAMEBUFFER, id);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TAO, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

            glBindFramebuffer(GL_FRAMEBUFFER, boundFBO);
        }

        ~Framebuffer() {
            deleteBuffers();
        };

        inline void bind(GLenum options = GL_FRAMEBUFFER) {
            // check if the framebuffer actually exists
            if (id == 0) {
                return;
            }

            glBindFramebuffer(options, id);
        };

        inline void unbind() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        };

        void changeDimentions(size_t newWidth, size_t newHeight);

        // Read pixel data and return a buffer, depending on the options argument, the buffer size can either be width x height x 3 if GL_RGB is selected or width x height x 4 if GL_RGBA is selected
        inline void Framebuffer::readData(uint8_t* data, GLenum bytes_per_pixel = 4) {
            glReadPixels(0, 0, (GLsizei) width, (GLsizei) height, bytes_per_pixel == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
        }

        inline uint32_t getWidth() {
            return width;
        };
        
        inline uint32_t getHeight() {
            return height;
        };
};

class MultisampledFramebuffer : public Framebuffer {
    private:
        GLuint samples;

    public:
        MultisampledFramebuffer(size_t _width, size_t _height, GLuint samples) : samples(samples) {
            width = _width;
            height = _height;
            if (samples == 0) {
                return;
            }
            
            glGenFramebuffers(1, &id);

            glGenTextures(1, &TAO);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, TAO);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, (GLsizei) width, (GLsizei) height, GL_TRUE);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glGenRenderbuffers(1, &RBO);
            glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, (GLsizei) width, (GLsizei) height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            // Same as for the textures, keep the reference to the last bound framebuffer
            GLint boundFBO;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &boundFBO);

            glBindFramebuffer(GL_FRAMEBUFFER, id);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, TAO, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

            glBindFramebuffer(GL_FRAMEBUFFER, boundFBO);
        };

        void changeDimentions(size_t newWidth, size_t newHeight);
}; 


#endif
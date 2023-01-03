
#include "Framebuffer.hpp"

void Framebuffer::deleteBuffers() {
    if (TAO != 0) {
        glDeleteTextures(1, &TAO);
    }

    if (RBO != 0) {
        glDeleteRenderbuffers(1, &RBO);
    }

    if (id != 0) {
        glDeleteFramebuffers(1, &id);
    }
}

void Framebuffer::changeDimentions(size_t newWidth, size_t newHeight) {
    // Change the value of the internal dimentions variables
    width = newWidth;
    height = newHeight;

    // Create new buffers/attachmets, make a new FBO and then substitute the current FBO and its attachments with the newly generated ones 
    GLuint newId = 0;
    GLuint newTAO = 0;
    GLuint newRBO = 0;

    glGenFramebuffers(1, &newId);   
    
    // Keep the refernce to the last bound texture to rebind it after the creation of the texture attachment
    GLint boundTex;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTex);

    glGenTextures(1, &newTAO);
    glBindTexture(GL_TEXTURE_2D, newTAO);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei) width, (GLsizei) height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, boundTex);

    glGenRenderbuffers(1, &newRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, newRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei) width, (GLsizei) height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Same as for the textures, keep the reference to the last bound framebuffer
    GLint boundFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &boundFBO);

    glBindFramebuffer(GL_FRAMEBUFFER, newId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, newTAO, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, newRBO);

    glViewport(0, 0, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, boundFBO);
    
    deleteBuffers();

    TAO = newTAO;
    RBO = newRBO;
    id = newId;
}

void MultisampledFramebuffer::changeDimentions(size_t newWidth, size_t newHeight) {
    // Change the value of the internal dimentions variables
    width = newWidth;
    height = newHeight;

    // Create new buffers/attachmets, make a new FBO and then substitute the current FBO and its attachments with the newly generated ones 
    GLuint newId = 0;
    GLuint newTAO = 0;
    GLuint newRBO = 0;

    glGenFramebuffers(1, &newId);

    // Keep the refernce to the last bound texture to rebind it after the creation of the texture attachment
    GLint boundTex;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTex);

    glGenTextures(1, &newTAO);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, newTAO);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, (GLsizei) width, (GLsizei) height, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, boundTex);

    glGenRenderbuffers(1, &newRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, newRBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, (GLsizei) width, (GLsizei) height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Same as for the textures, keep the reference to the last bound framebuffer
    GLint boundFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &boundFBO);

    glBindFramebuffer(GL_FRAMEBUFFER, newId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, newTAO, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, newRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, boundFBO);

    glViewport(0, 0, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, boundFBO);
    
    deleteBuffers();

    TAO = newTAO;
    RBO = newRBO;
    id = newId;
}


#ifndef VERTEXBUFFER_HPP
#define VERTEXBUFFER_HPP

#include <glad/glad.h>

class VertexBuffer {
    private:
        GLuint m_id;
        size_t m_data_size;

    public:
        VertexBuffer(uint8_t* data_ptr, size_t data_size, GLenum options = GL_STATIC_DRAW) : m_data_size(data_size) {
            glGenBuffers(1, &m_id);

            bind();

            glBufferData(GL_ARRAY_BUFFER, data_size, (void*)data_ptr, options);
        
            unbind();
        }

        inline void setAttribPointer(GLuint index, GLuint size, GLenum type, GLint stride, GLintptr offset, GLboolean normalized = false) {
            glVertexAttribPointer(index, size, type, normalized, stride, (void*)offset);
            glEnableVertexAttribArray(index);
        }

        inline void bind() { glBindBuffer(GL_ARRAY_BUFFER, m_id); }
        inline void unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
};

#endif
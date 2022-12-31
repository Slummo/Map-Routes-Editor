
#ifndef VERTEXARRAY_HPP
#define VERTEXARRAY_HPP

#include <glad/glad.h>

class VertexArray {
    private:
        GLuint m_id;

    public:
        VertexArray() {
            glGenVertexArrays(1, &m_id);
        }

        inline void bind() { glBindVertexArray(m_id); };
        inline void unbind() { glBindVertexArray(0); };


};

#endif
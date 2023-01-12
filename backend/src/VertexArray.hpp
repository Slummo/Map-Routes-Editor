
#ifndef VERTEXARRAY_HPP
#define VERTEXARRAY_HPP

#include "Buffer.hpp"
#include "Debug.hpp"
#include <vector>
#include <memory>
#include <glad/glad.h>

class VertexArray {
    private:
        std::vector<std::shared_ptr<VertexBuffer>> m_vertexBuffers;
        std::shared_ptr<IndexBuffer> m_indexBuffer;
        GLuint m_id;

    public:
        VertexArray() {
            glGenVertexArrays(1, &m_id);
        };

        void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
        void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);
        inline const std::shared_ptr<IndexBuffer>& getIndexBuffer() const { return m_indexBuffer; } 
        inline const std::shared_ptr<VertexBuffer>& getVertexBuffer() const { return m_vertexBuffers[0]; }

        inline void bind() { glBindVertexArray(m_id); };
        inline void unbind() { glBindVertexArray(0); };
};

#endif
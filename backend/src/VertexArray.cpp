
#include "VertexArray.hpp"

void VertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {
    glBindVertexArray(m_id);
    vertexBuffer->bind();

    uint32_t index = 0;
    auto layout = vertexBuffer->getLayout();
    for (const auto& element : layout) {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, element.getElementCount(), ShaderDataTypeToGLType(element.type), element.normalized ? GL_TRUE : GL_FALSE, layout.getStride(), (void*) element.offset);
        index++;
    }

    m_vertexBuffers.push_back(vertexBuffer);
}

void VertexArray::setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {
    glBindVertexArray(m_id);
    indexBuffer->bind();

    m_indexBuffer = indexBuffer;
}
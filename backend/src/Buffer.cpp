
#include "Buffer.hpp"

void VertexBufferLayout::CalculateOffsetsAndStride() {
    uint32_t offset = 0;
    for (auto& element : m_bufferElements) {
        element.offset = offset;
        offset += element.size;
        m_stride += element.size;
    }
}
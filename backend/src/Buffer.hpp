
#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <glad/glad.h>
#include <vector>
#include "Program.hpp"

struct VertexBufferElement {
    std::string name;
    ShaderDataType type;
    uint32_t offset;
    uint32_t size;
    bool normalized;

    VertexBufferElement(const std::string& _name, ShaderDataType _type, bool _normalized) : 
    name(_name), type(_type), size(ShaderDataTypeToSize(_type)), offset(0), normalized(_normalized) {};

    uint32_t getElementCount() {
        switch (type) {
            case ShaderDataType::Float:     return 1;
            case ShaderDataType::Float2:    return 2;
            case ShaderDataType::Float3:    return 3;
            case ShaderDataType::Float4:    return 4;
            case ShaderDataType::Mat2:      return 2 * 2;
            case ShaderDataType::Mat3:      return 3 * 3;
            case ShaderDataType::Mat4:      return 4 * 4;
            case ShaderDataType::Int:       return 1;
            case ShaderDataType::Uint:      return 1;

            defualt: std::cout << "[ERROR] Type not supported yet" << std::endl;
        }

        return 0;
    }
};

class VertexBufferLayout {
    private:
        std::vector<VertexBufferElement> m_bufferElements;
        uint32_t m_stride = 0;

        void CalculateOffsetsAndStride();

    public:
        VertexBufferLayout() {}

        VertexBufferLayout(const std::initializer_list<VertexBufferElement>& elements) : m_bufferElements(elements) {
            CalculateOffsetsAndStride();
        }

        inline const std::vector<VertexBufferElement> getElements() const { return m_bufferElements; } 
        inline uint32_t getStride() const { return m_stride; }

        std::vector<VertexBufferElement>::iterator begin() { return m_bufferElements.begin(); }
        std::vector<VertexBufferElement>::iterator end() { return m_bufferElements.end(); }
};

class VertexBuffer {
    private:
        GLuint m_id;
        VertexBufferLayout m_bufferLayout;

    public:
        VertexBuffer(void* data_ptr, size_t data_size, GLenum options = GL_STATIC_DRAW) {
            glGenBuffers(1, &m_id);
            glBindBuffer(GL_ARRAY_BUFFER, m_id);
            glBufferData(GL_ARRAY_BUFFER, data_size, data_ptr, options);
        }

        VertexBuffer(void* data_ptr, size_t data_size, const VertexBufferLayout layout, GLenum options = GL_STATIC_DRAW) {
            glGenBuffers(1, &m_id);
            glBindBuffer(GL_ARRAY_BUFFER, m_id);
            glBufferData(GL_ARRAY_BUFFER, data_size, data_ptr, options);

            setLayout(layout);
        }

        inline void bind() { glBindBuffer(GL_ARRAY_BUFFER, m_id); }
        inline void unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

        inline void setLayout(const VertexBufferLayout layout) { m_bufferLayout = layout; }

        inline const VertexBufferLayout& getLayout() const { return m_bufferLayout; }        
};

class IndexBuffer {
    private:
        GLuint m_id;
        size_t m_count;

    public:
        IndexBuffer (void* data_ptr, size_t count) : m_count(count) {
            glGenBuffers(1, &m_id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data_ptr, GL_STATIC_DRAW);
        }

        inline void bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id); }
        inline void unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

        inline size_t getCount() { return m_count; }
};

#endif

#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <glad/glad.h>
#include <string>
#include <iostream>
#include <fstream>

typedef GLuint Shader;

std::string readFile(const char* filepath);

GLuint loadProgram(const char* vertex_shader, const char* fragment_shader, Shader& vShader, Shader& fShader, bool verbose);

enum class ShaderDataType {
    None = 0, Float, Float2, Float3, Float4, Mat2, Mat3, Mat4, Int, Uint
};

static uint32_t ShaderDataTypeToSize(ShaderDataType t) {
    switch (t) {
        case ShaderDataType::Float:     return 4;
        case ShaderDataType::Float2:    return 4 * 2;
        case ShaderDataType::Float3:    return 4 * 3;
        case ShaderDataType::Float4:    return 4 * 4;
        case ShaderDataType::Mat2:      return 4 * 2 * 2;
        case ShaderDataType::Mat3:      return 4 * 3 * 3;
        case ShaderDataType::Mat4:      return 4 * 4 * 4;
        case ShaderDataType::Int:       return 4;
        case ShaderDataType::Uint:      return 4;

        defualt: std::cout << "[ERROR] Type not supported yet" << std::endl;
    }

    return 0;
}

static GLenum ShaderDataTypeToGLType(ShaderDataType t) {
    switch(t) {
        case ShaderDataType::Float:     return GL_FLOAT;
        case ShaderDataType::Float2:    return GL_FLOAT_VEC2;
        case ShaderDataType::Float3:    return GL_FLOAT_VEC3;
        case ShaderDataType::Float4:    return GL_FLOAT_VEC4;
        case ShaderDataType::Mat2:      return GL_FLOAT_MAT2;
        case ShaderDataType::Mat3:      return GL_FLOAT_MAT3;
        case ShaderDataType::Mat4:      return GL_FLOAT_MAT4;
        case ShaderDataType::Int:       return GL_INT;
        case ShaderDataType::Uint:      return GL_UNSIGNED_INT;

        defualt: std::cout << "[ERROR] Type not supported yet" << std::endl;
    }

    return 0;
}

class Program {
    private: 
        GLuint m_program;
        Shader m_vertShader;
        Shader m_fragShader;
    
    public:
        Program(const char* vertShader, const char* fragShader, bool debug = false) { load(vertShader, fragShader, debug); };
        Program(Program &p) : m_program(p.m_program) {};

        void load(const char* vertShader, const char* fragShader, bool debug = false) { m_program = loadProgram(vertShader, fragShader, m_vertShader, m_fragShader, debug); }
        void bind() { glUseProgram(m_program); };
        void unbind() { glUseProgram(0); };
        GLuint getUniformLocaiton(const char* uniform_name) { return glGetUniformLocation(m_program, uniform_name); };
        Shader getVertShader() { return m_vertShader; };
        Shader getFragShader() { return m_fragShader; };
};


#endif
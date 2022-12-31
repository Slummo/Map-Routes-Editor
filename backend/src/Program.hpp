
#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <glad/glad.h>
#include <string>
#include <iostream>
#include <fstream>

typedef GLuint Shader;

std::string readFile(const char* filepath);

GLuint loadProgram(const char* vertex_shader, const char* fragment_shader, Shader& vShader, Shader& fShader, bool verbose);

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

#include "Program.h"

std::string readFile(const char* filepath) {
    std::fstream file(filepath);

    if (!file.is_open()) {
        return "";
    }

    file.seekg(0, std::ios::end);
    uint32_t fSize = file.tellg();

    file.seekg(0, std::ios::beg);
    char* buffer = (char*)calloc(fSize + 1, sizeof(char));
    
    file.read(buffer, fSize);
    std::string fString(buffer);
    free(buffer);

    return fString;
}

GLuint loadProgram(const char* vertex_shader, const char* fragment_shader, Shader& vShader, Shader& fShader, bool verbose) {
    GLint result = GL_FALSE;
    int logLength;
    char* log;

    std::string vertShr = readFile(vertex_shader);
    std::string fragShr = readFile(fragment_shader);

    const char* vertShrSrc = vertShr.c_str();
    const char* fragShrSrc = fragShr.c_str();

    Shader vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    vShader = vertexShader;

    glShaderSource(vertexShader, 1, &vertShrSrc, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);
    if (verbose)
        printf("[STATUS]: %s\n", result == GL_TRUE ? "SUCCESS" : "FAILED");
    if (logLength > 1 && verbose) {
        log = (char*)malloc(logLength);
        glGetShaderInfoLog(vertexShader, logLength, NULL, log);
        printf("[INFO]: %s\n", log);
        free(log);
    }

    Shader fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    fShader = fragmentShader;

    glShaderSource(fragmentShader, 1, &fragShrSrc, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);
    if (verbose)
        printf("[STATUS]: %s\n", result == GL_TRUE ? "SUCCESS" : "FAILED");
    if (logLength > 1 && verbose) {
        log = (char*)malloc(logLength);
        glGetShaderInfoLog(fragmentShader, logLength, NULL, log);
        printf("[INFO]: %s\n", log);
        free(log);
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
    glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
    if (verbose)
        printf("[STATUS]: %s\n", result == GL_TRUE ? "SUCCESS" : "FAILED");
    if (logLength > 1 && verbose) {
        log = (char*)malloc(logLength);
        glGetProgramInfoLog(shaderProgram, logLength, NULL, log);
        printf("[INFO]: %s\n", log);
        free(log);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (!result) {
        printf("[ERROR] Failed to load the program, exiting...");
        exit(EXIT_FAILURE);
    }

    return shaderProgram;
}
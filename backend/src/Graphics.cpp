
#include "Graphics.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

GLFWwindow* gl_renderWndw(const char* name, Vec2u size) {

    GLFWwindow* window = glfwCreateWindow(size.first, size.second, name, NULL, NULL);

    if (window == NULL) {
        printf("\n[ERROR] Error in creating the window, exiting...");
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("\n[ERROR] Error loading the glad library, exiting...");
        exit(EXIT_FAILURE);
    }

    glViewport(0, 0, size.first, size.second);
    glfwSetFramebufferSizeCallback(window, frameResizeCallback);

    return window;
}

void frameResizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void _CheckGLError(const char* file, int line)
{
    GLenum err ( glGetError() );

    while ( err != GL_NO_ERROR )
    {
        std::string error;
        switch ( err )
        {
            case GL_INVALID_OPERATION:  error="INVALID_OPERATION";      break;
            case GL_INVALID_ENUM:       error="INVALID_ENUM";           break;
            case GL_INVALID_VALUE:      error="INVALID_VALUE";          break;
            case GL_OUT_OF_MEMORY:      error="OUT_OF_MEMORY";          break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
        }
        std::cout << "GL_" << error.c_str() << " - " << file << ":" << line << std::endl;
        err = glGetError();
    }
    return;
}

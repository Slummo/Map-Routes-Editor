
#include "Graphics.hpp"

GLFWwindow* gl_renderWndw(const char* name, Vec2u size) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(size.first, size.second, name, NULL, NULL);

    if (window == NULL) {
        printf("\n[ERROR] Error in creating the window, exiting...");
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGL(); 

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("\n[ERROR] Error loading the glad library, exiting...");
        exit(EXIT_FAILURE);
    }

    glViewport(0, 0, size.first, size.second);

    return window;
}

void clearColor(Color color) {
    glClearColor((GLfloat) color.r/255, (GLfloat) color.g/255, (GLfloat) color.b/255, (GLfloat) color.a/255);
}

VertexArray createGrid(float gridSize, int tiles) { 
  std::vector<glm::vec3> vertices;
  std::vector<glm::uvec2> indices;

  for (int i = -(tiles/2); i < (tiles/2 + 1); i++) {
    for (int j = -(tiles/2); j < (tiles/2 + 1); j++) {
      vertices.push_back(glm::vec3(((float) gridSize / tiles) * j, 0, ((float) gridSize / tiles) * i));
    }
  }

  for (int i = 0; i < tiles + 1; i++) {
    indices.push_back(glm::uvec2((tiles + 1) * i, (tiles+1) * i + tiles));
  }
  for (int i = 0; i < tiles + 1; i++) {
    indices.push_back(glm::uvec2(i, (tiles + 1) * tiles + i));
  }

  std::shared_ptr<VertexBuffer> gridVBO = std::make_shared<VertexBuffer>((void*)glm::value_ptr(vertices[0]), vertices.size() * sizeof(glm::vec3));
  VertexBufferLayout layout = {
    {"vertCoord", ShaderDataType::Float3, false}
  };
  gridVBO->setLayout(layout);

  std::shared_ptr<IndexBuffer> gridEBO = std::make_shared<IndexBuffer>((void*)glm::value_ptr(indices[0]), indices.size() * 2);

  VertexArray gridVAO;
  gridVAO.addVertexBuffer(gridVBO);
  gridVAO.setIndexBuffer(gridEBO);

  return gridVAO;
}

VertexArray createQuad(float quadSize) {
  std::vector<GLfloat> quadVertices{
    -1.f,  0.f, -1.f, 
    -1.f,  0.f,  1.f, 
    1.f, 0.f,  1.f, 

    1.f,  0.f, -1.f, 
    1.f,  0.f,  1.f, 
    -1.f,  0.f, -1.f, 
  };

  std::vector<GLuint> indexes{
    1, 2, 3,
    4, 5, 6
  };

  for (int i = 0; i < quadVertices.size(); i++) {
      quadVertices[i] = quadVertices[i] * quadSize;
  }


  std::shared_ptr<VertexBuffer> quadVBO = std::make_shared<VertexBuffer>((void*)quadVertices.data(), (size_t) quadVertices.size() * sizeof(GLfloat));
  VertexBufferLayout quadVBOLayout = {
    { "vertCoord", ShaderDataType::Float3, false}
  };
  quadVBO->setLayout(quadVBOLayout);

  std::shared_ptr<IndexBuffer> quadIndexes = std::make_shared<IndexBuffer>((void*)indexes.data(), indexes.size());

  VertexArray quadVAO;
  quadVAO.addVertexBuffer(quadVBO);
  quadVAO.setIndexBuffer(quadIndexes);

  return quadVAO;
}
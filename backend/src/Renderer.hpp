
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "VertexArray.hpp"
#include "Program.hpp"
#include "Color.hpp"
#include "Camera.hpp"
#include <glad/glad.h>

class Renderer {
    private:
        Camera m_Camera;
        glm::vec3 m_CameraPosition;
        glm::vec3 m_CameraTarget;


    public:

        void Clear(Color& color) const;
        void Draw(VertexArray& VAO, IndexBuffer& EBO, Program& program) const;
};

#endif
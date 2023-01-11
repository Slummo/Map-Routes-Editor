
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

class Camera {
    private:
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
        glm::vec3 position;
        glm::vec3 target;
        
        glm::vec3 up = glm::vec3(0, 1, 0);

        size_t screenWidth;
        size_t screenHeight;

        GLfloat FOV;
        GLfloat nearClip;
        GLfloat farClip;
    private:

        void UpdateView();
        void UpdateProjection();

    public:
        Camera(GLuint width, GLuint height, GLfloat nPlane, GLfloat fPlane, glm::vec3 pos, glm::vec3 _target, glm::vec3 up = glm::vec3(0, 1, 0), GLfloat fov = 45.f) : screenWidth(width), screenHeight(height), nearClip(nPlane), farClip(fPlane), position(pos), target(_target), up(up), FOV(fov) {};

        inline void setSize(glm::vec2 size) { screenWidth = size[0]; screenHeight = size[1]; };
        inline void setSize(size_t newWidth, size_t newHeight) { screenWidth = newWidth; screenHeight = newHeight; };

        inline void setPosition(glm::vec3 newPos) { position = newPos; };
        inline void setTarget(glm::vec3 newTarget) { target = newTarget; };
        inline void setFov(GLfloat fov) { FOV = fov; };

        inline void updateMatrices() { UpdateView(); UpdateProjection(); };

        inline glm::mat4 getViewProjection() const { return projectionMatrix * viewMatrix; };
        inline glm::vec3 getPosition() const { return position; };
        inline glm::vec3 getTarget() const { return target; }
};

#endif
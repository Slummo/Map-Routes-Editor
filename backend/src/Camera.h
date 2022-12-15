
#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <utility>
#include <glad/glad.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "Program.h"

class Camera {
    private:
        glm::vec3 m_position;
        glm::vec3 m_orientation;

        bool firstClick = true;

        const glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
        
        int m_width, m_height;

        float m_speed;
        float m_sens;

    public:
        Camera(int width, int height, glm::vec3 position, glm::vec3 orientation, float cam_speed = 0.005, float cam_sens = 100.0f) : m_width(width), m_height(height), m_position(position), m_orientation(orientation), m_speed(cam_speed), m_sens(cam_sens) {};

        void Matrix(float FOV, float nP, float fP, Program& program, const char* uniform_name);
        void Input(GLFWwindow* window);
        void setSize(glm::vec2 size);
};

#endif
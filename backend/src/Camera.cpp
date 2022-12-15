
#include "Camera.h"

void Camera::Matrix(float FOV, float nP, float fP, Program& program, const char* uniform_name) {
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    view = glm::lookAt(m_position, m_position + m_orientation, _up);
    projection = glm::perspective(glm::radians(FOV), (float)(m_width/m_height), nP, fP);

    glUniformMatrix4fv(program.getUniformLocaiton(uniform_name), 1, GL_FALSE, glm::value_ptr(projection * view));
}

void Camera::Input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        m_position += m_speed * m_orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        m_position += m_speed * -glm::normalize(glm::cross(m_orientation, _up));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        m_position -= m_speed * m_orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        m_position += m_speed * glm::normalize(glm::cross(m_orientation, _up));
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        m_position += m_speed * -_up;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        m_position += m_speed * _up;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (firstClick) {
            glfwSetCursorPos(window, m_width/2, m_height/2);
            firstClick = false;
        }

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        float rotX = m_sens * (float)(xpos - (m_width/2)) / m_width;
        float rotY = m_sens * (float)(ypos - (m_height/2)) / m_height;

        glm::vec3 new_orientation = glm::rotate(m_orientation, glm::radians(-rotY), glm::normalize(glm::cross(m_orientation, _up)));
        
        m_orientation = new_orientation;
        m_orientation = glm::rotate(m_orientation, glm::radians(-rotX), _up);

        glfwSetCursorPos(window, m_width/2, m_height/2);
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        firstClick = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void Camera::setSize(glm::vec2 size) {
    m_width = size.x;
    m_height = size.y;
}
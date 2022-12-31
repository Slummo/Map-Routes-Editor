
#include "Camera.hpp"

void Camera::UpdateView() {
    viewMatrix = glm::lookAt(position, position + orientation, up);
}

void Camera::UpdateProjection() {
    projectionMatrix = glm::perspective(glm::radians(FOV), (GLfloat) screenWidth / screenHeight, nearClip, farClip);
}
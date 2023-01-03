
#include "Camera.hpp"

void Camera::UpdateView() {
    viewMatrix = glm::lookAt(position, orientation, up);
}

void Camera::UpdateProjection() {
    projectionMatrix = glm::perspective(glm::radians(FOV), (GLfloat) screenWidth / screenHeight, nearClip, farClip);
}
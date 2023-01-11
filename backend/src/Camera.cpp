
#include "Camera.hpp"

void Camera::UpdateView() {
    viewMatrix = glm::lookAt(position, target - position, up);
}

void Camera::UpdateProjection() {
    projectionMatrix = glm::perspective(glm::radians(FOV), (GLfloat) screenWidth / screenHeight, nearClip, farClip);
}
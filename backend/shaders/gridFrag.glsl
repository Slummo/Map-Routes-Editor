#version 330 core

out vec4 color;

varying vec3 worldPos;
uniform vec3 camPos;

void main() {
    float dist = pow(worldPos.x - camPos.x, 2.0) + pow(worldPos.z - camPos.z, 2.0);
    color = vec4(1, 1, 1, 4/sqrt(dist));
}
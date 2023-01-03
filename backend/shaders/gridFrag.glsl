#version 330 core

out vec4 color;

in vec3 pos;
varying vec3 worldPos;

void main() {
    float dist = pow(worldPos.x - pos.x, 2.0) + pow(worldPos.z - pos.z, 2.0);
    color = vec4(1, 1, 1, 1);
}
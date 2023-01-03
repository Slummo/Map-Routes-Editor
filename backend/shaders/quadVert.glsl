#version 330 core

layout (location = 0) in vec3 vertCoord;

uniform mat4 camMatrix;

void main() {
    gl_Position = camMatrix * vec4(vertCoord.x * 100, vertCoord.y, vertCoord.z * 100, 1.0);
}
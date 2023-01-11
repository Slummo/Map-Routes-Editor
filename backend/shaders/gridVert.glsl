#version 330 core

layout (location = 0) in vec3 vertCoord;

uniform mat4 camMatrix;

varying vec3 worldPos;

void main() {
    vec3 realPos = vertCoord * 1;
    gl_Position = camMatrix * vec4(realPos, 1.0);
    worldPos = vertCoord.xyz;
}
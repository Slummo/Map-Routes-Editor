#version 330 core

out vec4 color;

varying vec3 worldPos;
uniform float time;

void main() {
    color = vec4(1, 1, 1, time/100);
}
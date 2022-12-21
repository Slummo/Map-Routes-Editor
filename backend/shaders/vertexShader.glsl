#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec3 FPos;
out vec2 TexCoord;

uniform mat4 transform;
uniform mat4 camMatrix;

void main() {
    FPos = aPos;
    TexCoord = aTexCoord;

    gl_Position = transform * vec4(aPos, 1.0f);
}
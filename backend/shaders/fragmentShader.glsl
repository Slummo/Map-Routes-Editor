#version 330 core

out vec4 FragColor;
in vec3 FPos;
in vec2 TexCoord;

uniform sampler2D mTexture;

void main() {
    FragColor = texture(mTexture, TexCoord);
}
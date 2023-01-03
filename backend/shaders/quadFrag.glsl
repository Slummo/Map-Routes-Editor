#version 330 core

out vec4 color;

uniform vec2 resolution;

void main() {
    vec2 normalized_pos = gl_FragCoord.xy / resolution;
    color = vec4(normalized_pos.x, normalized_pos.y, 1, 1);
}
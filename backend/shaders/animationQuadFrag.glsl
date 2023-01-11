#version 330 core

out vec4 color;

uniform vec2 resolution;
uniform float time;

void main() {
    vec2 normalized_pos = gl_FragCoord.xy / resolution;
    color = vec4(normalized_pos, 1, (time - 160) / 400);
}
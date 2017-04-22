#version 330 core
uniform float max;
uniform float min;

in vec2 uv;
in float height;
out vec4 color;

void main() {
    float h = (height - min) / (max - min);
    color = vec4(h, h, h, 1);
}

#version 330 core
uniform float max;
uniform float min;

in vec2 uv;
in float height;
out vec3 color;

void main() {
    float h = (height - min) / (max - min);
    color = vec3(h);
}

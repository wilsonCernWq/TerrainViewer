#version 430 core
uniform float max;
uniform float min;
in  float h;
in  vec2  uv;
out vec4  color;

void main() {
    color = vec4(0.0,0.0,0.0,1.0);
    color.r = (h - min) / (max - min);
}

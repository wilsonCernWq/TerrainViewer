#version 430 core
in  vec2 vpoint;
out vec2 uv;

void main() {
    uv = (vpoint + vec2(1.0, 1.0)) * 0.5;
    gl_Position = vec4(vpoint, 0.0, 1.0);
}

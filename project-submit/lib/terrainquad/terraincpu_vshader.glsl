#version 330 core
in vec3 vpoint;
out vec2 uv;
out float height;

void main() {
    height = vpoint.z;
    uv = (vpoint.xy + vec2(1.0, 1.0)) * 0.5;
    gl_Position = vec4(vpoint.x, vpoint.y, 0.0, 1.0);
}

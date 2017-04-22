#version 330 core
uniform mat4 MVP;
uniform vec2 origin;
uniform float range;

in vec2 vtexcoord;
in vec3 vpoint;
out vec2 uv;
out vec3 pos;

void main() {
    gl_Position = MVP * vec4(vpoint, 1.0);
    uv = vtexcoord / range + origin;
    pos = vpoint;
}

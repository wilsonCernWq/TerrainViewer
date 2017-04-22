#version 330 core
uniform sampler2D tex;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec2 origin;
uniform float maxH;
uniform float terR;

in vec2 position;

void main() {
    vec2 uv = (position + vec2(1.0, 1.0)) / 2.0 / terR + origin;
    vec4 pos = vec4(position.x, position.y, maxH * texture(tex, uv).r, 1.0);
    gl_Position = P * V * M * pos;
}

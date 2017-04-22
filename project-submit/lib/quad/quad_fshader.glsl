#version 330 core
uniform sampler2D tex;

in vec2 uv;
out vec4 color;

void main() {
    if (texture(tex,uv).a < 0.5) {
        discard;
    } else {
        color = vec4(texture(tex,uv).rgb, 1);
    }
}

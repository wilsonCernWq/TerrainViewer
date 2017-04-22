#version 330 core
uniform sampler2D tex;

in vec2 uv;
out vec3 color;

void main() {
    if (texture(tex,uv).a < 0.5) {
        discard;
    } else {
        color = texture(tex,uv).rgb;
    }
}

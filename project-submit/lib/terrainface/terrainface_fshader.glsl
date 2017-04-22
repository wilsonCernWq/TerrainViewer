#version 330 core
uniform sampler2D tex;
uniform vec2 origin;
uniform float maxHeight;
uniform float range;
uniform int side;

in vec2 uv;
in vec3 pos;
out vec3 color;

void main() {
    float height;
    if (side == 0) {
        height = texture(tex, vec2(uv[0],0.0/range+origin[1])).r * maxHeight;
    } else if (side == 1) {
        height = texture(tex, vec2(uv[0],1.0/range+origin[1])).r * maxHeight;
    } else if (side == 2) {
        height = texture(tex, vec2(0.0/range+origin[0],uv[1])).r * maxHeight;
    } else if (side == 3) {
        height = texture(tex, vec2(1.0/range+origin[0],uv[1])).r * maxHeight;
    } else {
        height = 0.01;
    }

    if (pos[1] > height && (side == 0 || side == 1)){
        discard;
    } else if (pos[0] > height && (side == 2 || side == 3)){
        discard;
    } else {
        color = vec3(0,0,1);
    }
}

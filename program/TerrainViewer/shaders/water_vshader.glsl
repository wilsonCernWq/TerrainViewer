#version 330 core
uniform mat4  MV;
uniform mat4  proj;
uniform float water_level;
in  vec2 vpoint;
out vec2 uv;
out vec4 vpoint_mv;

void main() {
    uv = 0.5 + 0.5 * vpoint;
    vpoint_mv = MV * vec4(vpoint.x, vpoint.y, water_level, 1.0);
    gl_Position = proj * vpoint_mv;
}

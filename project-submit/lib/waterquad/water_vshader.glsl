#version 330 core
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float water_height;
in vec3 vpoint;
in vec2 vtexcoord;
out vec2 uv;
out vec4 vpoint_mv;

void main() {
    mat4 MV = view * model;
    vec3 pos = vec3(vpoint.x, vpoint.y, water_height);
    vpoint_mv = MV * vec4(pos, 1.0);
    gl_Position = projection * vpoint_mv;
    uv = vtexcoord;
}

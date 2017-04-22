#version 330 core
uniform sampler2D tex;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 offset_matrix;
uniform vec3 light_pos;
uniform vec2 origin;
uniform float maxH;
uniform float terR;
uniform bool inverted;
uniform float waterHeight;

in vec2 position;
out vec2 uv;
out float height;
out vec3 vnormal;
out vec3 normal_mv;
out vec3 light_dir;
out vec4 shadow_coord;

void main() {
    uv = (position + vec2(1.0, 1.0)) / 2.0 / terR + origin;
    height = maxH * texture(tex, uv).r;
    float inverted_height = height;
    if (inverted) {
        inverted_height = height * -1;
        inverted_height += waterHeight*2;
    }
    vec4 pos = vec4(position.x, position.y, inverted_height, 1.0);

    //-- Calculate normal: X axis
    vec2 xp = vec2(-5.0 / 512.0, 0.0);
    vec2 xf = vec2( 5.0 / 512.0, 0.0);
    float xh = 512.0 * (texture(tex, uv + xf/ 10.0).r - texture(tex, uv + xp/ 10.0).r);
    vec3 dx = vec3(10.0, 0.0, xh);

    //-- Calculate normal: Y axis
    vec2 yp = vec2(0.0, -5.0 / 512.0);
    vec2 yf = vec2(0.0,  5.0 / 512.0);
    float yh = 512.0 * (texture(tex, uv + yf/ 10.0).r - texture(tex, uv + yp/ 10.0).r);
    vec3 dy = vec3(0.0, 10.0, yh);

    mat4 MV = V * M;
    vec4 vpoint_mv = MV * pos;
    vnormal = cross(dx, dy);
    normal_mv = (inverse(transpose(MV)) * vec4(vnormal, 1.0)).xyz;
    light_dir = (MV * vec4(light_pos, 1.0)).xyz - vpoint_mv.xyz;
    gl_Position = P * vpoint_mv;
    shadow_coord = offset_matrix * pos;
}

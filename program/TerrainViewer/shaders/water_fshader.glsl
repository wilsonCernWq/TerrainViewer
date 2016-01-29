#version 330 core
uniform sampler2D tex_reflex;
uniform sampler2D tex_normal;
uniform mat4 MV;

uniform vec3  ka;
uniform vec3  kd;
uniform vec3  Ia;
uniform vec3  Id;
uniform vec3  lightp;
uniform vec3  water_color;
uniform float time;

in vec2 uv;
in vec4 vpoint_mv;
out vec4 color;

const float M_PI = 3.1415926535897932384626433832795;

void main() {
    vec3 L = normalize(lightp);

    //---> Diffuse Shading
    vec3 normal1 = texture2D(tex_normal, uv * 6.0 + time / 10.0).rgb;
    vec3 N1 = normalize(normal1);

    vec3 normal2 = texture2D(tex_normal, vec2(4 * uv.x + time/8, 4 * uv.y) + time/8).rgb;
    vec3 N2 = normalize(normal2);

    //---> Mix colors
    vec3 blue1 = water_color * 1.5 * Id * max(dot(L, N1), 0.2);
    vec3 blue2 = water_color * 1.5 * Id * max(dot(L, N2), 0.2) / 2;

    //vec3 lookAt = vec3(MV[0][2], MV[1][2], MV[2][2]);
    //float alpha = acos(dot(normalize(lookAt), vec3(0, 0, 1))) / (2 * M_PI);

    vec3 k = mix(blue1, blue2, 0.5);
    color = vec4(k, 0.5);
}

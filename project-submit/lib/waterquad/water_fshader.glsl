#version 330 core
uniform sampler2D tex_reflex;
uniform sampler2D tex_normal;
uniform mat4 model;
uniform mat4 view;
uniform vec3 Id;
uniform vec3 light_pos;
uniform vec3 water_color;
uniform float time;

out vec4 color;
in vec2 uv;
in vec4 vpoint_mv;

#define M_PI 3.1415926535897932384626433832795

void main() {
    ivec2 window_size = textureSize(tex_reflex, 0);
    float u = gl_FragCoord.x / window_size.x;
    float v = gl_FragCoord.y / window_size.y;

    //---> Diffuse Shading
    mat4 MV = view * model;
    vec4 vnormal = vec4(texture(tex_normal, uv * 6 + time / 10).rgb, 1.0);
    vec3 normal_mv = (inverse(transpose(MV)) * vnormal).xyz;
    vec3 light_dir = (MV * vec4(light_pos, 1.0)).xyz - vpoint_mv.xyz;
    vec3 N = normalize(normal_mv);
    vec3 L = normalize(light_dir);

    // Normal 2
    vec2 _uv = uv * 4;
    _uv = vec2(_uv[0] + time / 4, _uv[1] + time / 8);
    vec4 vnormal_2 = vec4(texture(tex_normal, _uv).rgb, 1.0);
    vec3 normal2_mv = (inverse(transpose(MV)) * vnormal_2).xyz;
    vec3 N2 = normalize(normal2_mv);

    //---> Mix colors
    vec3 blueish = 1.5 * Id * water_color * max(dot(L, N), 0.2);
    vec3 blueish2 = 1.5 * (Id / 2) * water_color * max(dot(L, N2), 0.2);

    vec3 lookAt = vec3(MV[0][2], MV[1][2], MV[2][2]);
    float alpha = acos(dot(normalize(lookAt), vec3(0, 0, 1))) / (2 * M_PI);

    vec3 tcolor = mix(blueish, blueish2, 0.5);
    tcolor = mix(texture(tex_reflex, vec2(u, v)).rgb, tcolor, 1-alpha);
    color = vec4(tcolor, 0.9);
}

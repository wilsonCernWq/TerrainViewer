#version 330 core
uniform sampler2D tex;
uniform sampler2D tex_normal;
uniform mat4 model;
uniform mat4 view;
uniform vec3 Id;
uniform vec3 light_pos;
uniform vec3 water_color;
uniform float time;

out vec3 color;
in vec2 uv;
in vec4 vpoint_mv;

void main() {
    ivec2 window_size = textureSize(tex, 0);
    float u = gl_FragCoord.x / window_size.x;
    float v = gl_FragCoord.y / window_size.y;

    //---> Diffuse Shading
    mat4 MV = view * model;
    vec4 vnormal = vec4(texture(tex_normal, uv * 10 + time / 10).rgb, 1.0);
    vec3 normal_mv = (inverse(transpose(MV)) * vnormal).xyz;
    vec3 light_dir = (MV * vec4(light_pos, 1.0)).xyz - vpoint_mv.xyz;
    vec3 N = normalize(normal_mv);
    vec3 L = normalize(light_dir);

    //---> Mix colors
    color = 1.5 * Id * water_color * max(dot(L, N), 0.0);
    color = mix(texture(tex, vec2(u, v)).rgb, color, 0.6);
    //color = texture(tex, vec2(u, v)).rgb;
    //color = vec3(1,0,0);
}

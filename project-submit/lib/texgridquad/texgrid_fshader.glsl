#version 330 core
const float M_PI = 3.14;
uniform sampler2D shadow_map;
uniform sampler2D color_tex0; // Glass
uniform sampler2D color_tex1; // Rock
uniform sampler2D color_tex2; // Snow
uniform sampler2D color_tex3; // Sand
uniform sampler2D color_tex4; // Forest
uniform sampler2D color_tex5; // Weedy Glass
uniform vec3 Id;
uniform bool inverted;
uniform float waterHeight;

in vec2 uv;
in float height;
in vec3 vnormal;
in vec3 normal_mv;
in vec3 light_dir;
in vec4 shadow_coord;
out vec4 color;

// Poisson disk sample locations.
vec2 poisson_disk[16] = vec2[](
   vec2(-0.94201624, -0.39906216),
   vec2(0.94558609, -0.76890725),
   vec2(-0.094184101, -0.92938870),
   vec2(0.34495938, 0.29387760),
   vec2(-0.91588581, 0.45771432),
   vec2(-0.81544232, -0.87912464),
   vec2(-0.38277543, 0.27676845),
   vec2(0.97484398, 0.75648379),
   vec2(0.44323325, -0.97511554),
   vec2(0.53742981, -0.47373420),
   vec2(-0.26496911, -0.41893023),
   vec2(0.79197514, 0.19090188),
   vec2(-0.24188840, 0.99706507),
   vec2(-0.81409955, 0.91437590),
   vec2(0.19984126, 0.78641367),
   vec2(0.14383161, -0.14100790)
);

float alpha(vec3 v, float w, float tor){
    float x = sqrt(v[1] * v[1] + v[0] * v[0]);
    if(abs(x) >= (w + tor)) {
        return 0.0;
    } else if(abs(x) <= w) {
        return 1.0;
    } else {
        return 0.5 + 0.5 * cos(M_PI * (abs(x) - w) / tor);
    }
}

float hfit(float x, float w, float tor){
    if(x >= (w + tor)) {
        return 1.0;
    } else if(x <= w) {
        return 0.0;
    } else {
        return 0.5 - 0.5 * cos(M_PI * (x - w) / tor);
    }
}

float lfit(float x, float w, float tor){
    if(x >= (w + tor)) {
        return 0.0;
    } else if(x <= w) {
        return 1.0;
    } else {
        return 0.5 * cos(M_PI * (x - w) / tor) + 0.5;
    }
}

float rfit(float x, float h, float l, float tor){
    return lfit(x,h,tor) * hfit(x,l,tor);
}

void main() {
    if (inverted && height <= waterHeight) {
        discard;
    }

    vec3 n = normalize(vnormal);
    // Diffuse Shading & fog
    vec3 N = normalize(normal_mv);
    vec3 L = normalize(light_dir);
    float cosTheta = dot(L, N);
    float diffusion = max(cosTheta, 0.0)*0.5 + 0.5;
    // shadow
    float shadow = 1.0;
    float bias = clamp(0.005*tan(acos(cosTheta)), 0.0, 0.002);
    for (int i=0; i < 16;i++){
        float z1 = texture(shadow_map, shadow_coord.xy + poisson_disk[i]/1000.0).r;
        float z2 = shadow_coord.z;
        if (z1 <  z2 - bias){
           shadow -= 0.045;
        }
    }
    // calculate filter
    float h = height;
    float fg = 0.5 * rfit(h,0.6,0.30,0.1) * alpha(n, 0.6, 0.1);
    float ff = 0.5 * rfit(h,0.3,0.15,0.1) * alpha(n, 0.6, 0.1);
    float fw = 0.5 * rfit(h,0.4,0.30,0.1) * alpha(n, 0.6, 0.1);
    //float fh = clamp(hfit(h,0.6,0.001)*alpha(n,0.8,0.02)+rfit(h,0.6,0.4,0.001)*alpha(n,0.4,0.02),0.0,1.0);
    float fh = clamp(hfit(h,0.3,0.001)*alpha(n, 2 * h - 0.4,0.02),0.0,1.0);
    float fl = lfit(h,0.2,0.02);

    // rock and glass
    vec3 tColor = mix(texture(color_tex1, 60.0 * uv).rgb, texture(color_tex0, 50.0 * uv).rgb, fg);
    tColor = mix(tColor, texture(color_tex4, 50.0 * uv).rgb, ff);
    tColor = mix(tColor, texture(color_tex5, 50.0 * uv).rgb, fw);
    // snow
    tColor = mix(tColor, texture(color_tex2, 30.0 * uv).rgb, fh);
    // sand
    tColor = mix(tColor, texture(color_tex3, 140.0 * uv).rgb, fl);

    tColor = mix(tColor, tColor * Id * shadow * diffusion, 0.7);
    color = vec4(tColor, 1);
}


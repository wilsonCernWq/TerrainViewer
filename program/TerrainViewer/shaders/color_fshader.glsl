#version 430 core
const float M_PI = 3.1415926535;
const vec2  poissonDisk16[16] = vec2[](
   vec2(-0.94201624, -0.39906216),
   vec2( 0.94558609, -0.76890725),
   vec2(-0.09418410, -0.92938870),
   vec2( 0.34495938,  0.29387760),
   vec2(-0.91588581,  0.45771432),
   vec2(-0.81544232, -0.87912464),
   vec2(-0.38277543,  0.27676845),
   vec2( 0.97484398,  0.75648379),
   vec2( 0.44323325, -0.97511554),
   vec2( 0.53742981, -0.47373420),
   vec2(-0.26496911, -0.41893023),
   vec2( 0.79197514,  0.19090188),
   vec2(-0.24188840,  0.99706507),
   vec2(-0.81409955,  0.91437590),
   vec2( 0.19984126,  0.78641367),
   vec2( 0.14383161, -0.14100790)
);

uniform sampler2D shadow_map;
uniform sampler2D pnoise_map;
uniform sampler2D color_tex0; // High Lands
uniform sampler2D color_tex1; // Fault Zone
uniform sampler2D color_tex2; // Snowy_Mud
uniform sampler2D color_tex3; // Sahara
uniform sampler2D color_tex4; // Forested Mountain Side
uniform sampler2D color_tex5; // Weedy Lawn

const vec3 color0 = vec3(0.36,0.40,0.31);
const vec3 color1 = vec3(0.28,0.26,0.20);
const vec3 color2 = vec3(0.90,0.90,0.90);
const vec3 color3 = vec3(0.81,0.70,0.46);
const vec3 color4 = vec3(0.39,0.54,0.35);
const vec3 color5 = vec3(0.00,0.00,0.00);

const float zNear =  10.0;
const float zFar  = 300.0;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 Ia;
uniform vec3 Id;

uniform float sandH; // sand/sea level
uniform float snowH; // snow level
uniform float maxH; // maximum height
uniform float terR;  // terrain display range
uniform float terS;  // local texture scale

in  vec2  uv;
in  vec3  normal;
in  vec3  fpoint;
in  vec3  flight;
in  vec4  sh_Position;
out vec4  color;

float perlinNoise(float depth, float level, float freq)
{
    float pnoise;
    if (depth > level) {
        pnoise = 0.0;
    } else {
        pnoise = (1.0 - depth / level) * (2.0 * texture2D(pnoise_map, freq * uv).r - 1.0);
    }
    return pnoise;
}

vec3 mixTexColor(float depth, float freq, float l1, float l2, sampler2D tex, vec3 color)
{
    vec3 k;
    if (depth > l2) {
        k = color;
    } else if (depth > l1) {
        k = mix(texture2D(tex, freq * uv).rgb, color, (depth - l1)/(l2 - l1));
    } else {
        k = texture2D(tex, freq * uv).rgb;
    }
    return k;
}

void main() {
    // Calculate variables
    float depth = 2.0 * zNear * zFar / (zNear + zFar - (2.0 * gl_FragCoord.z - 1.0) * (zFar - zNear)) / (zFar - zNear);
    vec3  k = vec3(0.5,0.5,0.5);

    //*/ Phone Shading  ------------------------------------------------------------------
    vec3  N = normalize(normal);
    vec3  L = normalize(flight);
    float dCoef = max(dot(L, N), 0.0);
    vec3  ShadingColor = mix(Ia * ka, Id * kd * dCoef, 0.7);
    //*/
    //*/ Texture -------------------------------------------------------------------------
    //--- noise ---
    float height = fpoint.z * terS;

    //--- texture ---
    if (height > 0.1 * perlinNoise(depth, 0.6, 2.0) * maxH + snowH)
    {
        // draw snow
        k = mixTexColor(depth, 8.0, 0.3, 0.5, color_tex2, color2);
    }
    else if (height > 0.05 * perlinNoise(depth, 0.6, 2.0) * maxH + sandH)
    {
        // draw rock
        if (abs(N.z) < 0.15) {
            k = mixTexColor(depth, 4.0, 0.5, 0.7, color_tex1, color1);
        }
        // draw glass
        else if (abs(N.z) < 0.4)
        {
            k = mixTexColor(depth, 6.0, 0.5, 0.7, color_tex0, color0);
        }
        else
        {
            k = mixTexColor(depth, 6.0, 0.5, 0.7, color_tex4, color4);
        }
    }
    else
    {
        // draw sand
        k = mixTexColor(depth, 2.0, 0.5, 0.7, color_tex3, color3);
    }
    //*/
    //*/ Shadow --------------------------------------------------------------------------
    vec3  ShadowColor = vec3(0.0,0.0,0.0);
    float p = 1.0;
    float b = 0.002 * clamp(tan(acos(dCoef)), 0.0, 1.0);

    for (int i = 0; i < 16; ++i)
    {
        vec2  d  = poissonDisk16[i] * terS / terR / 500.0;
        float z1 = texture2D(shadow_map, sh_Position.xy + d).r;
        float z2 = sh_Position.z;
        if (z1 <  z2 - b) { p -= 1.0 / 32.0; }
    }
    //*/
    //*/ Sum up all effects  -------------------------------------------------------------
    //k = 0.5*N+0.5;
    //k = 0.5*L+0.5;
    //k = vec3(dCoef,dCoef,dCoef);
    k = mix(ShadingColor, k, 0.7);
    k = mix(ShadowColor , k, p  );
    color = vec4(k, 1.0);
    //*/
}


#version 430 core
uniform sampler2D height_map;
uniform mat4 MVP;
uniform vec2 origin_vcoord;
uniform float maxH;
uniform float terR;
uniform float terS; // local texture scale

in vec3 vpoint;

float getHeight(const vec2 pos)
{
    return texture2D(height_map, pos).r * maxH / terS;
}

void main() {
    vec2 uv = 0.5 * (vpoint.xy * terS + origin_vcoord) / terR + vec2(0.5, 0.5);
    vec4 pcoord = vec4(vpoint.x, vpoint.y, getHeight(uv), 1.0);
    gl_Position = MVP * pcoord;
}

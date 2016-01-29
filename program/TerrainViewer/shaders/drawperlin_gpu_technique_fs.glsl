#version 430 core
//
// GLSL textureless classic 2D noise "cnoise",
// with an RSL-style periodic variant "pnoise".
// Author:  Stefan Gustavson (stefan.gustavson@liu.se)
// Version: 2011-08-22
//
// Many thanks to Ian McEwan of Ashima Arts for the
// ideas for permutation and gradient selection.
//
// Copyright (c) 2011 Stefan Gustavson. All rights reserved.
// Distributed under the MIT license. See LICENSE file.
// https://github.com/ashima/webgl-noise
//
vec4 mod289(vec4 x)
{
    return x - floor(x / 289.0) * 289.0;
}

vec4 permute(vec4 x)
{
    return mod289(x * x * 34.0 + x);
}

vec4 taylorInvSqrt(vec4 r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}

float fade(float t)
{
    return t*t*t*(t*(t*6.0-15.0)+10.0);
}

float dade(float t)
{
    return t*t*(t*(30.0*t+60.0)+30.0);
}

float noise(vec2 P)
{
    vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
    vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
    Pi = mod289(Pi); // To avoid truncation effects in permutation
    vec4 ix = Pi.xzxz;
    vec4 iy = Pi.yyww;
    vec4 fx = Pf.xzxz;
    vec4 fy = Pf.yyww;

    vec4 i = permute(permute(ix) + iy);

    vec4 tx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
    vec4 gx = tx - floor(tx + 0.5);
    vec4 gy = abs(tx) - 0.5;

    vec2 g00 = vec2(gx.x,gy.x);
    vec2 g10 = vec2(gx.y,gy.y);
    vec2 g01 = vec2(gx.z,gy.z);
    vec2 g11 = vec2(gx.w,gy.w);

    vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
    g00 *= norm.x;
    g01 *= norm.y;
    g10 *= norm.z;
    g11 *= norm.w;

    float n00 = dot(g00, vec2(fx.x, fy.x));
    float n10 = dot(g10, vec2(fx.y, fy.y));
    float n01 = dot(g01, vec2(fx.z, fy.z));
    float n11 = dot(g11, vec2(fx.w, fy.w));

    float k0 = n00;
    float k1 = n10 - n00;
    float k2 = n01 - n00;
    float k3 = n00 - n10 - n01 + n11;

    float ux = fade(Pf.x);
    float uy = fade(Pf.y);
    float nv = k0 + ux * k1 + uy * k2 + ux * uy * k3;
    return 2.3 * nv;
}

uniform float span;
in  vec2 uv;
out vec4 color;

void main() {
    color = vec4(0.0,0.0,0.0,1.0);
    color.r = 0.5 + 0.5 * noise(span * uv);
}

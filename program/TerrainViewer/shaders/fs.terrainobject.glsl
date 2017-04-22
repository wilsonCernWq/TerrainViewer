#version 430 core

layout(location = 0) out vec4 color;

in float gsHeight;
in vec2 gsTexcoord;
in vec3 gsWorldCoordNormal;
in vec3 gsCameraCoordNormal;
in vec3 gsCameraCoordPosition;
in vec4 gsShadowCoordPosition;

uniform int sysmode;
uniform int geomode;
uniform vec3 lightDirCameraCoord;
uniform sampler2D noise;
uniform sampler2D shadowtex;
uniform sampler2D featuretex0; // Highlands
uniform sampler2D featuretex1; // Fault_Zone
uniform sampler2D featuretex2; // Snowy_Mud
uniform sampler2D featuretex3; // Sahara
uniform sampler2D featuretex4; // Forested_Mountainside
uniform sampler2D featuretex5; // Weedy_Lawn
uniform float znear;
uniform float zfar;

uniform float sandH; // sand/sea level
uniform float snowH; // snow level
uniform float maxH;  // maximum height
uniform vec2 origin;

const float M_PI = 3.1415926535;
const vec2  poissonDisk16[16] = vec2[](
	vec2(-0.94201624, -0.39906216),
	vec2(0.94558609, -0.76890725),
	vec2(-0.09418410, -0.92938870),
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

const float A = 0.1, D = 0.6, S = 0.6, Ns = 5.0f;
const vec3 color0 = vec3(0.36, 0.40, 0.31);
const vec3 color1 = vec3(0.28, 0.26, 0.20);
const vec3 color2 = vec3(0.90, 0.90, 0.90);
const vec3 color3 = vec3(0.81, 0.70, 0.46);
const vec3 color4 = vec3(0.39, 0.54, 0.35);
const vec3 color5 = vec3(0.00, 0.00, 0.00);

const vec3 colorKa = vec3(0.5, 0.5, 0.5);
const vec3 colorKs = vec3(1.0, 1.0, 1.0);

float perlinNoise(float depth, float level, float freq)
{
	float pnoise;
	if (depth > level) {
		pnoise = 0.0;
	}
	else {
		pnoise = (1.0 - depth / level) * (2.0 * texture2D(noise, origin + freq * gsTexcoord).r - 1.0);
	}
	return pnoise;
}

vec3 mixTexColor(float depth, float freq, float l1, float l2, sampler2D tex, vec3 color)
{
	vec3 k;
	if (depth > l2) {
		k = color;
	}
	else if (depth > l1) {
		k = mix(texture2D(tex, origin + freq * gsTexcoord).rgb, color, (depth - l1) / (l2 - l1));
	}
	else {
		k = texture2D(tex, origin + freq * gsTexcoord).rgb;
	}
	return k;
}

void main() {

	if (sysmode == 1 || geomode == 1) {
		color = vec4(1.0f, 1.0f, 1.0f, 1.0f); // white debug color
	}
	else {

		vec3 curr_color;
		// compute vectors
		vec3 L = normalize(lightDirCameraCoord);
		vec3 N = normalize(gsCameraCoordNormal);
		vec3 V = -normalize(gsCameraCoordPosition);
		vec3 H = normalize(L + V);
		// compute angles
		float dcos = max(0.0f, dot(N, L));
		float scos = max(0.0f, dot(H, N));

		//--- shadow
		float shadow_percent = 1.0;
		float shadow_bias = 0.002 * clamp(tan(acos(dcos)), 0.0, 1.0);
		vec3 shadow_coord = gsShadowCoordPosition.xyz / gsShadowCoordPosition.w;
		vec3 shadow_color = vec3(0.0, 0.0, 0.0);
		for (int i = 0; i < 16; ++i)
		{
			vec2  d = poissonDisk16[i] / 500.0;
			float z1 = texture2D(shadowtex, shadow_coord.xy + d).r;
			float z2 = shadow_coord.z;
			if (z1 <  z2 - shadow_bias) { shadow_percent -= 1.0 / 32.0; }
		}
		
		//--- shading
		if (shadow_percent < 0.0001f) { // skip shading if pixel is 100% in shadow
			curr_color = mix(colorKa * A, shadow_color, 1 - shadow_percent);
		}
		else {
			float depth = 2.0f * znear * zfar / (znear + zfar - (2.0 * gl_FragCoord.z - 1.0) * (zfar - znear)) / (zfar - znear);
			float height = gsHeight;

			//--- phone shading
			vec3 shading_color = colorKa * A + dcos * (colorKa * D + colorKs * S * pow(scos, Ns));

			//--- texture feature
			vec3  feature_color = vec3(0.5, 0.5, 0.5);
			float shuffle = perlinNoise(depth, 0.8, 10.0);
			if (height > 0.1 * shuffle * maxH + snowH)
			{
				// draw snow
				feature_color = mixTexColor(depth, 0.5, 0.3, 0.5, featuretex2, color2);
			}
			else if (height > 0.01 * shuffle * maxH + sandH)
			{
				vec3 normal = normalize(gsWorldCoordNormal);
				// draw rock
				if (abs(normal.z) < 0.15) {
					feature_color = mixTexColor(depth, 4.0, 0.5, 0.7, featuretex1, color1);
				}
				// draw glass
				else if (abs(normal.z) < 0.9)
				{
					feature_color = mixTexColor(depth, 6.0, 0.5, 0.7, featuretex0, color0);
				}
				else
				{
					feature_color = mixTexColor(depth, 10.0, 0.5, 0.7, featuretex4, color4);
				}
			}
			else
			{
				// draw sand
				feature_color = mixTexColor(depth, 2.0f, 0.5f, 0.7f, featuretex3, color3);
			}

			//--- mix color
			curr_color = feature_color;
			curr_color = mix(curr_color, shading_color, 0.2);
			curr_color = mix(curr_color, shadow_color, 1 - shadow_percent);

		}
		color = vec4(curr_color, 1.0f);
	}
}

#version 430 core

#define SZ gl_MaxPatchVertices

layout(quads, equal_spacing, cw) in;

patch in float gl_TessLevelOuter[4];
patch in float gl_TessLevelInner[2];

in vec2 tcPosition[SZ];
in vec2 tcTexcoord[SZ];
in float tcScale[SZ];

out float teHeight;
out vec2 teTexcoord;
out vec3 teWorldCoordPosition;
out vec3 teCameraCoordPosition;
out vec4 teShadowCoordPosition;

uniform mat4 sm;
uniform mat4 smv;
uniform mat4 smvp;
uniform mat4 shadowmvp;
uniform sampler2D noise;

uniform float O; // octave
uniform float P; // persistence
uniform float L; // lacunarity
uniform float G; // gain
uniform float B; // offset
uniform vec2 origin;

float getfmfractal(vec2 coord) 
{
	float A = 0.1f * P;     // amplititude
	float W = 1.0f;         // weight
	float S = 0.0f;         // signal
	float V = 0.0f;         // value
	vec2  x = coord / 32.0f + origin;
	// calculate fractal motion
	for (int i = 0; i < O; i++) {
		float z = texture2D(noise, x).r;
		// compute rmfractal height
		S = B - abs(z);
		S = S * S * W;
		W = min(S * G, 1.0f);
		V += S / A;
		// compute texture
		A *= P;
		x *= L;
	}
	return V;
}

vec2 getcoord(vec2 list[SZ]) 
{
	vec2 x0 = mix(list[0].xy, list[1].xy, gl_TessCoord.x);
	vec2 x1 = mix(list[2].xy, list[3].xy, gl_TessCoord.x);
	return mix(x0, x1, gl_TessCoord.y);
}

void main()
{
	// calculate coordinate
	vec3 position = vec3(getcoord(tcPosition), 1.0f);
	vec2 texcoord = getcoord(tcTexcoord);
	// compute fractal position
	position.z = getfmfractal(texcoord) / 100.0f;
	// apply changes
	teHeight = position.z;
	teTexcoord = texcoord;
	teWorldCoordPosition = position;
	teCameraCoordPosition = vec3(smv * vec4(position, 1.0f));
	teShadowCoordPosition = shadowmvp * sm * vec4(position, 1.0f);
	gl_Position = smvp * vec4(position, 1);
}

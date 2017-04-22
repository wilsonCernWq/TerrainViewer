#version 430 core

#define ID gl_InvocationID
#define SZ gl_MaxPatchVertices

layout(vertices = 4) out;

in vec2 vsPosition[SZ];
in vec2 vsTexcoord[SZ];
in vec3 vsCameraCoord[SZ];
in vec3 vsScreenCoord[SZ];
in int vsMark[SZ];
in float vsScale[SZ];

out vec2 tcPosition[SZ];
out vec2 tcTexcoord[SZ];
out float tcScale[SZ];

patch out float gl_TessLevelOuter[4];
patch out float gl_TessLevelInner[2];

uniform vec3 lodCenter; // can be useful
uniform mat4 v;
vec4 cameraCoordCenter;

float distCurve(float x)
{
	return 20.0f * exp(-x / 40.0f) + 1.0f;
}

float distFunc(int a, int b)
{
	vec3 point;
	float scale;
	if (vsMark[a] == 2 && vsMark[b] == 1) { 
		point = vsCameraCoord[a]; scale = 0.5f;
	}
	else if (vsMark[a] == 1 && vsMark[b] == 2) {
		point = vsCameraCoord[b]; scale = 0.5f;
	}
	else {
		point = (vsCameraCoord[a] + vsCameraCoord[b]) / 2.0f; 
		scale = 1.0f;
	}
	float screenLength = length(point - cameraCoordCenter.xyz); // distance to camera
	return round(distCurve(screenLength)) * 2 * scale;
}

bool testCoord()
{
  return 
	  (((-1.0f < vsScreenCoord[0].x && vsScreenCoord[0].x <= 1.0f) &&
	   (-1.0f < vsScreenCoord[0].y && vsScreenCoord[0].y <= 1.0f)) ||
	  ((-1.0f < vsScreenCoord[1].x && vsScreenCoord[1].x <= 1.0f) &&
	   (-1.0f < vsScreenCoord[1].y && vsScreenCoord[1].y <= 1.0f)) ||
	  ((-1.0f < vsScreenCoord[2].x && vsScreenCoord[2].x <= 1.0f) &&
	   (-1.0f < vsScreenCoord[2].y && vsScreenCoord[2].y <= 1.0f)) ||
	  ((-1.0f < vsScreenCoord[3].x && vsScreenCoord[3].x <= 1.0f) &&
	   (-1.0f < vsScreenCoord[3].y && vsScreenCoord[3].y <= 1.0f)));
}

void main()
{
	cameraCoordCenter = v * vec4(lodCenter, 1.0f);
	cameraCoordCenter = cameraCoordCenter / cameraCoordCenter.w;
	if (testCoord()) {
		tcPosition[ID] = vsPosition[ID];
		tcTexcoord[ID] = vsTexcoord[ID];
		tcScale[ID] = vsScale[ID];

		gl_TessLevelOuter[0] = distFunc(0, 2);
		gl_TessLevelOuter[1] = distFunc(1, 0);
		gl_TessLevelOuter[2] = distFunc(3, 1);
		gl_TessLevelOuter[3] = distFunc(2, 3);

		gl_TessLevelInner[0] = 0.5 * (gl_TessLevelOuter[0] + gl_TessLevelOuter[2]);
		gl_TessLevelInner[1] = 0.5 * (gl_TessLevelOuter[1] + gl_TessLevelOuter[3]);
	} 
	else {
		gl_TessLevelOuter[1] = 0;
		gl_TessLevelOuter[2] = 0;
		gl_TessLevelOuter[3] = 0;
		gl_TessLevelOuter[0] = 0;
		gl_TessLevelInner[0] = 0;
		gl_TessLevelInner[1] = 0;
	}
}

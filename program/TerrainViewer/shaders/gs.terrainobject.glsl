#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in float teHeight[];
in vec2 teTexcoord[];
in vec3 teWorldCoordPosition[];
in vec3 teCameraCoordPosition[];
in vec4 teShadowCoordPosition[];

out float gsHeight;
out vec2 gsTexcoord;
out vec3 gsWorldCoordNormal;
out vec3 gsCameraCoordNormal;
out vec3 gsCameraCoordPosition;
out vec4 gsShadowCoordPosition;

void main()
{
	gsHeight = teHeight[0];
	gsTexcoord = teTexcoord[0];
    gsCameraCoordPosition = teCameraCoordPosition[0];
	gsWorldCoordNormal = -normalize(cross(teWorldCoordPosition[1] - teWorldCoordPosition[0], teWorldCoordPosition[2] - teWorldCoordPosition[0]));
	gsCameraCoordNormal = -normalize(cross(teCameraCoordPosition[1] - teCameraCoordPosition[0], teCameraCoordPosition[2] - teCameraCoordPosition[0]));
	gsShadowCoordPosition = teShadowCoordPosition[0];
    gl_Position = gl_in[0].gl_Position; EmitVertex();

	gsHeight = teHeight[1];
	gsTexcoord = teTexcoord[1];
    gsCameraCoordPosition = teCameraCoordPosition[1];
	gsWorldCoordNormal = -normalize(cross(teWorldCoordPosition[2] - teWorldCoordPosition[1], teWorldCoordPosition[0] - teWorldCoordPosition[1]));
	gsCameraCoordNormal = -normalize(cross(teCameraCoordPosition[2] - teCameraCoordPosition[1], teCameraCoordPosition[0] - teCameraCoordPosition[1]));
	gsShadowCoordPosition = teShadowCoordPosition[1];
	gl_Position = gl_in[1].gl_Position; EmitVertex();

	gsHeight = teHeight[2];
	gsTexcoord = teTexcoord[2];
    gsCameraCoordPosition = teCameraCoordPosition[2];
	gsWorldCoordNormal = -normalize(cross(teWorldCoordPosition[0] - teWorldCoordPosition[2], teWorldCoordPosition[1] - teWorldCoordPosition[2]));
	gsCameraCoordNormal = -normalize(cross(teCameraCoordPosition[0] - teCameraCoordPosition[2], teCameraCoordPosition[1] - teCameraCoordPosition[2]));
	gsShadowCoordPosition = teShadowCoordPosition[2];
    gl_Position = gl_in[2].gl_Position; EmitVertex();

    EndPrimitive();
}

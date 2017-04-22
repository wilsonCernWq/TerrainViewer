#version 430 core

layout(location = 0) in vec4 Position;

out vec2 vsPosition;
out vec2 vsTexcoord;
out vec3 vsCameraCoord;
out vec3 vsScreenCoord;
out int  vsMark;
out float vsScale;

uniform mat4 smv;
uniform mat4 smvp;

void main() {
  vsPosition = Position.xy;
  vsTexcoord = Position.xy * 0.5 + 0.5;
  vsMark = int(round(Position.z));
  vsScale = Position.w;
  vec4 cameracoord = smv * vec4(Position.x, Position.y, 0.0f, 1.0f);
  vec4 screencoord = smvp * vec4(Position.x, Position.y, 0.0f, 1.0f);
  vsCameraCoord = cameracoord.xyz;
  vsScreenCoord = screencoord.xyz / screencoord.w;
}

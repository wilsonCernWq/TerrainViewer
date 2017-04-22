#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vnormal;

out vec3 fnormal;
uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(position, 1.0f);
	fnormal = vnormal;
}

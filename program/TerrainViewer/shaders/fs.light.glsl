#version 330 core

layout(location = 0) out vec4 color;
in vec3 fnormal;

void main() {
	color = vec4(fnormal, 1.0f);
}

#version 330 core

layout(location = 0) out vec4 color;
in vec3 ftexcoord;
uniform sampler2D tex;

void main() {
	color = vec4(vec3(texture(tex, ftexcoord.xy).r), 1.0f);
}

#version 330 core
uniform samplerCube tex;
in vec3 uv;
out vec4 color;

void main(){
    color = vec4(texture(tex, uv).rgb, 1.0);
}

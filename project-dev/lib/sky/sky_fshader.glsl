#version 330 core
uniform samplerCube tex;
in vec3 uv;
out vec3 color;

void main(){
    color = texture(tex, uv).rgb;
}

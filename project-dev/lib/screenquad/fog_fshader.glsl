#version 330 core
uniform sampler2D tex;
uniform sampler2D dis;
uniform vec3 fogcolor;
in vec2 uv;
out vec3 color;

float getfog(float distance)  {
   float fResult = exp(-pow(0.03 * distance, 2.0));
   fResult = 1.0 - clamp(fResult, 0.0, 1.0);
   return fResult;
}

void main() {
    float z = - 50 * 0.1 / (texture(dis, uv).r * (1000 - 0.1) - 1000);
    float fog = getfog(z);
    color = mix(texture(tex,uv).rgb, fogcolor, fog);
}


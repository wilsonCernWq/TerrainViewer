#version 330 core
uniform sampler2D tex;
uniform vec3 Ia, Id, Is;
uniform vec3 ka, kd, ks;
uniform float p;
in vec2 uv;
in vec3 normal_mv;
in vec3 light_mv;
in vec3 view_mv;
out vec3 color;


void main(){
    //--- Phong shading.
    vec3 Aproduct = Ia * ka;
    vec3 Dproduct = Id * kd;
    vec3 Sproduct = Is * ks;

    vec3 N = normalize(normal_mv);
    vec3 L = normalize(light_mv);
    vec3 V = normalize(view_mv);
    vec3 H = normalize(reflect(-L, N));

    /// 1) compute ambient term.
    vec3 A = Aproduct;

    /// 2) compute diffuse term.
    vec3 D = Dproduct * max(dot(L, N), 0.0);

    /// 3) compute specular term.
    vec3 S = Sproduct * pow(max(dot(N, H), 0.0), p);

    color = A + D + S;
    color = 0.7 * texture(tex, uv).rgb + 0.3 * color;
}

#version 330 core
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 Light;
uniform float time;

in vec3 vpoint;
in vec3 vnormal;
in vec2 vtexcoor;
out vec2 uv;
out vec3 normal_mv;
out vec3 light_mv;
out vec3 view_mv;

mat4 R(float degrees){
    mat4 R = mat4(1);
    float alpha = radians(degrees);
    R[0][0] =  cos(alpha);
    R[0][1] =  sin(alpha);
    R[1][0] = -sin(alpha);
    R[1][1] =  cos(alpha);
    return R;
}
mat4 T(float tx, float ty, float tz){
    mat4 T = mat4(1);
    T[3][0] = tx;
    T[3][1] = ty;
    T[3][2] = tz;
    return T;
}

void main(){
    mat4 MVP = Projection * View * Model;
    //gl_Position =  MVP * R(50*time) * T(1,0,0) * R(50*time)* vec4(vpoint,1); ///< spin-circ
    //gl_Position =  MVP * R(50*time)* vec4(vpoint,1); ///< spin
    gl_Position =  MVP * R(0) * vec4(vpoint,1); ///< still
    uv = vtexcoor;

    mat4 MV = View * Model;
    vec4 vpoint_mv = MV * vec4(vpoint, 1.0);

    //--- Phong shading.
    normal_mv = (inverse(transpose(MV)) * vec4(vnormal, 1.0)).xyz;
    light_mv = Light - vpoint_mv.xyz;
    view_mv =  -vpoint_mv.xyz;
}

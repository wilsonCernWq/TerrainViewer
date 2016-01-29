#version 430 core
uniform sampler2D height_map;
uniform float maxH; // maximum height
uniform float terR; // terrain display range
uniform float terS; // local texture scale
uniform vec3  lightp;
uniform mat4  vMVP;
uniform mat4  lMVP;
uniform vec2  origin_vcoord;
uniform vec2  origin_tcoord;

in  vec3  vpoint;
out vec2  uv;
out vec3  normal;
out vec3  fpoint;
out vec3  flight;
out vec4  sh_Position;

vec4 getHeight(const vec2 pos)
{
    vec4 data = texture2D(height_map, pos);
    data.r *= maxH / terS;
    return data;
}

const float dt = 0.001;

void main()
{
    // Calculate uv coordinate
    uv = 0.5 * (vpoint.xy * terS + origin_vcoord) / terR + vec2(0.5, 0.5);

    // sample from height map
    vec4 data = getHeight(uv);

    // calculate frag coordinate
    fpoint = vec3(vpoint.x, vpoint.y, data.r);

    // calculate frag normal
    normal = 2.0 * data.gba - 1.0;
    normal.xy *= maxH/normal.z;
    normal = normalize(normal);

    // calculate light position
    flight = lightp;// - fpoint;

    // calculate vertex position
    gl_Position = vMVP * vec4(fpoint, 1.0);
    sh_Position = lMVP * vec4(fpoint, 1.0);
    uv +=  0.5 * origin_tcoord * 32;
}

#version 430 core
uniform sampler2D reference;
uniform float P; // persistance
uniform float L; // lacunarity
uniform float O; // octaves
uniform float G; // gain
uniform float B; // offset
uniform vec2 origin;

in  vec2 uv;
out vec4 color;

void main() {
    const float dt = 1.0 / 512.0;
    color = vec4(0.0,0.0,0.0,1.0);
    float A = P / 10.0; // amplititude

    float W = 1.0;      // weight
    float S = 0.0;      // signal
    float V = 0.0;      // value

    vec4 dW = vec4(1.0,1.0,1.0,1.0); // weight
    vec4 dS = vec4(0.0,0.0,0.0,0.0); // signal
    vec4 dV = vec4(0.0,0.0,0.0,0.0); // value

    vec2  x = uv / 32.0 + 0.5 * origin;

    // calculate fractal motion
    for (int i = 0; i < O; i++)
    {
        // retrieve noise texture
        float z  = texture2D(reference, x).r;
        float z1 = texture2D(reference, x + vec2(dt,0.0)).r;
        float z2 = texture2D(reference, x + vec2(0.0,dt)).r;
        float z3 = texture2D(reference, x - vec2(dt,0.0)).r;
        float z4 = texture2D(reference, x - vec2(0.0,dt)).r;
        vec4 dz = vec4(z1,z2,z3,z4);

        // compute multi fractal
        S   = B - abs(z);
        S   = S * S * W;
        W   = min(S * G, 1.0);
        V  += S / A;

        dS  = B - abs(dz);
        dS  = dS * dS * dW;
        dW  = min(dS * G, 1.0);
        dV += dS / A;

        // compute texture
        A *= P;
        x *= L;
    }
    color.r   = V;
    color.gba = 0.5 + 0.5 * normalize(vec3(dV.z - dV.x,dV.w - dV.y, 4.0 * dt * dt));
}

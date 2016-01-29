#ifndef _LIGHT_H_
#define _LIGHT_H_
#include "../icg_helper.h"

/**
 * @brief The Light struct To define the position of light source
 */
struct Light
{
    float sharpness = 60.0;
    vec3  light_position = vec3(-2.0f, 8.0f, 4.0f);
    vec3  ka = vec3(1.0f, 1.0f, 1.0f); ///< Ambient   color
    vec3  kd = vec3(1.0f, 1.0f, 1.0f); ///< Diffusion color
    vec3  ks = vec3(1.0f, 1.0f, 1.0f); ///< Specular  color
    vec3  Ia = vec3(1.0f, 1.0f, 1.0f); ///< Ambient   intensity
    vec3  Id = vec3(1.0f, 1.0f, 1.0f); ///< Diffusion intensity
    vec3  Is = vec3(0.0f, 0.0f, 0.0f); ///< Specular  intensity
    mat4  offset;

    ///--- Constructor
    Light()
    {
        offset << 0.5f, 0.0f, 0.0f, 0.5f,
                  0.0f, 0.5f, 0.0f, 0.5f,
                  0.0f, 0.0f, 0.5f, 0.5f,
                  0.0f, 0.0f, 0.0f, 1.0f;
    }

    ///--- Setup Light position in shaders
    void setup(GLuint pid)
    {
        glUniform3fv(glGetUniformLocation(pid, "lightp"), ONE, light_position.data());
        glUniform3fv(glGetUniformLocation(pid, "ka"), ONE, ka.data());
        glUniform3fv(glGetUniformLocation(pid, "kd"), ONE, kd.data());
        glUniform3fv(glGetUniformLocation(pid, "Ia"), ONE, Ia.data());
        glUniform3fv(glGetUniformLocation(pid, "Id"), ONE, Id.data());
    }

    ///--- The light position is inconsistent between shading and shadow map
    vec3 shadow_map()
    {
        return vec3(light_position(0), light_position(2), -light_position(1));
    }
};

#endif // _LIGHT_H_

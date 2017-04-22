#ifndef LIGHT_H
#define LIGHT_H
#include "icg_common.h"

struct Light {
    vec3 light_pos = vec3(-2.0f, 8.0f, 4.0f);
    vec3 Id = vec3(1.0, 1.0, 1.0); ///< Diffusion intensity
    vec3 Is = vec3(1.0, 1.0, 1.0); ///< Specular intensity

    void setup(GLuint pid) {
        glUseProgram(pid);
        GLuint ps_id = glGetUniformLocation(pid, "light_pos");
        GLuint Id_id = glGetUniformLocation(pid, "Id");
        GLuint Is_id = glGetUniformLocation(pid, "Is");
        glUniform3fv(ps_id, ONE, light_pos.data());
        glUniform3fv(Id_id, ONE, Id.data());
        glUniform3fv(Is_id, ONE, Is.data());
    }
};
#endif

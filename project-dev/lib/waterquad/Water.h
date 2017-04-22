#ifndef WATER_H
#define WATER_H
#include "icg_common.h"
#include "../Light.h"
#include "../quad/Quad.h"

class Water : public Quad {
protected:
    GLuint _tex_normal; ///< Normal map
    Light* _sun;

public:
    vec3 water_color = vec3(0.3, 0.3, 0.5);
    float water_height = 0.0;

    Water(){
        _vshader = "lib/waterquad/water_vshader.glsl";
        _fshader = "lib/waterquad/water_fshader.glsl";
    }

    void setTexture(GLuint texture){ _tex = texture; }

    void setLight(Light& sun) {  _sun = &sun; }

    void setWaterHeight(float height) { water_height = height; }

    void ShaderTexture(GLuint program) {
        glGenTextures(1, &_tex_normal);
        glBindTexture(GL_TEXTURE_2D, _tex_normal);
        glfwLoadTexture2D("lib/waterquad/water_texture.tga", 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glUniform1i(glGetUniformLocation(program, "tex"), 0 /*GL_TEXTURE0*/);
        glUniform1i(glGetUniformLocation(program, "tex_normal"), 1 /*GL_TEXTURE1*/);
    }
           
    void draw(const mat4& model, const mat4& view, const mat4& projection){
        glUseProgram(_pid);
        glBindVertexArray(_vao);
        {
            /// @todo Bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _tex);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, _tex_normal);
            
            /// @todo Matrices
            GLuint model_id = glGetUniformLocation(_pid, "model");
            glUniformMatrix4fv(model_id, 1, GL_FALSE, model.data());
            GLuint view_id = glGetUniformLocation(_pid, "view");
            glUniformMatrix4fv(view_id, 1, GL_FALSE, view.data());
            GLuint projection_id = glGetUniformLocation(_pid, "projection");
            glUniformMatrix4fv(projection_id, 1, GL_FALSE, projection.data());

            /// @todo Light
            _sun->setup(_pid);

            /// @todo Time
            glUniform1f(glGetUniformLocation(_pid, "time"), glfwGetTime());

            /// @todo Parameters
            glUniform3fv(glGetUniformLocation(_pid, "water_color"), ONE, water_color.data());
            glUniform1f(glGetUniformLocation(_pid, "water_height"), water_height);
            
            /// @todo Draw
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        glBindVertexArray(0);        
        glUseProgram(0);
    }
};
#endif

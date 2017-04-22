#ifndef BORDER_H
#define BORDER_H
#include "icg_common.h"
#include "../quad/Quad.h"
#include "../Light.h"

class Terrainface: public Quad{
protected:
    GLuint _pid_depth;
    GLuint _vao_depth;
    GLfloat& maxHeight;
    GLfloat& displayRatio;
    GLfloat& terrainRange;

protected:
    void ShaderTexture(GLuint program) {
        glUniform1i(glGetUniformLocation(program, "tex"), 0 /*GL_TEXTURE0*/);
        check_error_gl();
    }

    void InitMembers(){
        _pid_depth = opengp::load_shaders("lib/terrainface/faceshadow_vshader.glsl",
                                          "lib/terrainface/faceshadow_fshader.glsl");
        if(!_pid_depth) { exit(EXIT_FAILURE); }
        ///////////////////////////////////////////////////////////////////////////////
        glUseProgram(_pid_depth);
        glGenVertexArrays(1, &_vao_depth);
        glBindVertexArray(_vao_depth);
        ///////////////////////////////////////////////////////////////////////////////
        ShaderParameters(_pid_depth);
        glUniform1i(glGetUniformLocation(_pid_depth, "tex"), 1 /*GL_TEXTURE1*/);
        ///////////////////////////////////////////////////////////////////////////////
        glBindVertexArray(0);
        glUseProgram(0);
        check_error_gl();
    }

public:
    void setTexture(GLuint texture) { _tex = texture; }

    Terrainface(GLfloat& maxH, GLfloat& disR, GLfloat& terR)
        : maxHeight(maxH), displayRatio(disR), terrainRange(terR){
        _vshader = "lib/terrainface/terrainface_vshader.glsl";
        _fshader = "lib/terrainface/terrainface_fshader.glsl";
    }

    void draw(const mat4& MVP, vec2& origin, int index){
        glUseProgram(_pid);
        glBindVertexArray(_vao);
        {
            ///////////////////////////////////////////////////////////////////////////////
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _tex);
            ///////////////////////////////////////////////////////////////////////////////
            glUniform1i(glGetUniformLocation(_pid, "side"), index);
            glUniform1f(glGetUniformLocation(_pid, "maxHeight"), maxHeight / displayRatio);
            glUniform1f(glGetUniformLocation(_pid, "range"), terrainRange / displayRatio);
            glUniform2fv(glGetUniformLocation(_pid, "origin"), 1, origin.data());
            glUniformMatrix4fv(glGetUniformLocation(_pid, "MVP"), 1, GL_FALSE, MVP.data());
            ///////////////////////////////////////////////////////////////////////////////
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            ///////////////////////////////////////////////////////////////////////////////
        }
        glBindVertexArray(0);
        glUseProgram(0);
        check_error_gl();
    }

    void drawDepth(const mat4& MVP, vec2& origin, int index){
        glUseProgram(_pid_depth);
        glBindVertexArray(_vao_depth);
        {
            ///////////////////////////////////////////////////////////////////////////////
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, _tex);
            ///////////////////////////////////////////////////////////////////////////////
            glUniform1i(glGetUniformLocation(_pid_depth, "side"), index);
            glUniform1f(glGetUniformLocation(_pid_depth, "maxHeight"), maxHeight / displayRatio);
            glUniform1f(glGetUniformLocation(_pid_depth, "range"), terrainRange / displayRatio);
            glUniform2fv(glGetUniformLocation(_pid_depth, "origin"), 1, origin.data());
            glUniformMatrix4fv(glGetUniformLocation(_pid_depth, "MVP"), 1, GL_FALSE, MVP.data());
            ///////////////////////////////////////////////////////////////////////////////
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            ///////////////////////////////////////////////////////////////////////////////
        }
        glBindVertexArray(0);
        glUseProgram(0);
        check_error_gl();
    }
};
#endif

#ifndef SCREENQUAD_H
#define SCREENQUAD_H
#include "icg_common.h"
#include "../quad/Quad.h"

class ScreenQuad: public Quad {
protected:
    GLuint _zbuffer;
    vec3 _fogcolor = vec3(1.0,1.0,1.0);

    /**
     * @brief ShaderTexture: Overridable function for initialize default texture
     */
    void ShaderTexture(GLuint program) {
        /// @todo Pass texture as uniforms
        glUniform1i(glGetUniformLocation(program, "tex"), 0 /* GL_TEXTURE0 */);
        glUniform1i(glGetUniformLocation(program, "dis"), 1 /* GL_TEXTURE1 */);
    }

public:
    ScreenQuad(){
        _vshader = "lib/screenquad/fog_vshader.glsl";
        _fshader = "lib/screenquad/fog_fshader.glsl";
    }

    /**
     * @brief setTexture Load/Assign texture from input
     * @param texture: texture Id
     */
    void setTexture(GLuint texture) { _tex = texture; }

    /**
     * @brief setZbuffer Load z-buffer
     * @param zbuffer
     */
    void setZbuffer(GLuint zbuffer) {  _zbuffer = zbuffer; }

    /**
     * @brief draw
     */
    void draw(){
        glUseProgram(_pid);
        glBindVertexArray(_vao);

        /// @todo Draw screen
        {
            /// @todo Set texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _tex);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, _zbuffer);

            /// @todo Set parameters
            glUniform3fv(glGetUniformLocation(_pid, "fogcolor"), 1, _fogcolor.data());

            /// @todo Draw
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        glBindVertexArray(0);        
        glUseProgram(0);
        check_error_gl();
    }
};
#endif

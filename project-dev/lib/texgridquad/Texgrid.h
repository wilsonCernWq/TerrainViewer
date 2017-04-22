#ifndef TEXGRID_H
#define TEXGRID_H
#include "icg_common.h"
#include "../quad/Quad.h"
#include "../Light.h"

class TexGrid: public Quad{
private:
    GLuint _pid_depth;
    GLuint _vao_depth;
    GLuint _num_indices;  ///< number of vertices to render
    GLuint _depth_buffer;
    GLuint _color_tex[6];  ///< color textures
    mat4 _offset;
private:
    Light& lightSource;
    GLfloat& maxHeight;
    GLfloat& displayRatio;
    GLfloat& terrainRange;
private:
    void ShaderParameters(GLuint program){
        std::vector<GLfloat> vertices;
        std::vector<GLuint> indices;
        /// @todo Grid size definition
        const float dim = 1024;
        GLfloat space = 2.0 / dim;
        /// @todo Vertex position of the triangles.
        for(int j = 0; j <= dim; j++) {
            for(int i = 0; i < dim; i++) {
                vertices.push_back(-1.0f + space * i); ///< x coord
                vertices.push_back(-1.0f + space * j); ///< y coord
            }
        }
        /// @todo Indices.
        for(int j = 0; j < dim; j++) {
            for(int i = 0; i < dim; i++) {
                int index = i;
                if (j % 2 == 1) {
                    index = dim - 1 - i;
                }
                indices.push_back((j+1) * dim + index);
                indices.push_back(j * dim + index);
            }
        }
        _num_indices = indices.size();
        /// @todo position buffer
        glBindBuffer(GL_ARRAY_BUFFER, genBuffer());
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, genBuffer());
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
        GLuint position_id = glGetAttribLocation(program, "position");
        glEnableVertexAttribArray(position_id);
        glVertexAttribPointer(position_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        check_error_gl();
    }

    void ShaderTexture(GLuint program) {
        /// @todo height texture
        glUniform1i(glGetUniformLocation(program, "tex"), 0 /*GL_TEXTURE0*/);
        glUniform1i(glGetUniformLocation(program, "shadow_map"), 1);

        /// @todo color texture
        glGenTextures(6, _color_tex);

        glBindTexture(GL_TEXTURE_2D, _color_tex[0]);
        glfwLoadTexture2D("ext/Mossy_Rock.tga", 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glUniform1i(glGetUniformLocation(program, "color_tex0"), 2);

        glBindTexture(GL_TEXTURE_2D, _color_tex[1]);
        glfwLoadTexture2D("ext/Fault_Zone.tga", 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glUniform1i(glGetUniformLocation(program, "color_tex1"), 3);

        glBindTexture(GL_TEXTURE_2D, _color_tex[2]);
        glfwLoadTexture2D("ext/Snow.tga", 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glUniform1i(glGetUniformLocation(program, "color_tex2"), 4);

        glBindTexture(GL_TEXTURE_2D, _color_tex[3]);
        glfwLoadTexture2D("ext/Sahara.tga", 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glUniform1i(glGetUniformLocation(program, "color_tex3"), 5);

        glBindTexture(GL_TEXTURE_2D, _color_tex[4]);
        glfwLoadTexture2D("ext/Forested_Mountainside.tga", 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glUniform1i(glGetUniformLocation(program, "color_tex4"), 6);

        glBindTexture(GL_TEXTURE_2D, _color_tex[5]);
        glfwLoadTexture2D("ext/Weedy_Lawn.tga", 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glUniform1i(glGetUniformLocation(program, "color_tex5"), 7);
        check_error_gl();
    }

public:
    TexGrid(Light& sun, GLfloat& maxH, GLfloat& disR, GLfloat& terR)
        :lightSource(sun), maxHeight(maxH), displayRatio(disR), terrainRange(terR){
        _vshader = "lib/texgridquad/texgrid_vshader.glsl";
        _fshader = "lib/texgridquad/texgrid_fshader.glsl";
        _offset << 0.5f, 0.0f, 0.0f, 0.5f,
                   0.0f, 0.5f, 0.0f, 0.5f,
                   0.0f, 0.0f, 0.5f, 0.5f,
                   0.0f, 0.0f, 0.0f, 1.0f;
    }

    float getWaterHeight() { return 0.2 * maxHeight / displayRatio; }

    void setTexture(GLuint texture) { _tex = texture; }

    void setDepthBuffer(GLuint buffer) { _depth_buffer = buffer; }

    void draw(const mat4& T, const mat4& M, const mat4& V, const mat4& P, const mat4& LVP, vec2& origin){
        glUseProgram(_pid);
        glBindVertexArray(_vao);
        {
            /// @todo Bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _tex);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, _depth_buffer);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, _color_tex[0]);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, _color_tex[1]);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, _color_tex[2]);
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, _color_tex[3]);
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, _color_tex[4]);
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, _color_tex[5]);
            /// @todo Light and Material
            lightSource.setup(_pid);
            /// @todo Bind Matrices
            mat4 offset_mvp = _offset * LVP * M;
            mat4 MT = T * M;
            glUniform1f(glGetUniformLocation(_pid, "maxH"), maxHeight / displayRatio);
            glUniform1f(glGetUniformLocation(_pid, "terR"), terrainRange / displayRatio);
            glUniform2fv(glGetUniformLocation(_pid, "origin"), 1, origin.data());
            glUniformMatrix4fv(glGetUniformLocation(_pid, "M"), 1, GL_FALSE, MT.data());
            glUniformMatrix4fv(glGetUniformLocation(_pid, "V"), 1, GL_FALSE, V.data());
            glUniformMatrix4fv(glGetUniformLocation(_pid, "P"), 1, GL_FALSE, P.data());
            glUniformMatrix4fv(glGetUniformLocation(_pid, "offset_matrix"), 1, GL_FALSE, offset_mvp.data());
            glDrawElements(GL_TRIANGLE_STRIP, _num_indices, GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);        
        glUseProgram(0);
        check_error_gl();
    }

    void InitMembers(){
        _pid_depth = opengp::load_shaders("lib/texgridquad/gridshadow_vshader.glsl",
                                          "lib/texgridquad/gridshadow_fshader.glsl");
        if(!_pid_depth) { exit(EXIT_FAILURE); }
        glUseProgram(_pid_depth);
        glGenVertexArrays(1, &_vao_depth);
        glBindVertexArray(_vao_depth);
        ShaderParameters(_pid_depth);
        glUniform1i(glGetUniformLocation(_pid_depth, "tex"), 8 /*GL_TEXTURE8*/);
        glBindVertexArray(0);
        glUseProgram(0);
        check_error_gl();
    }

    void drawDepth(const mat4& M, const mat4& V, const mat4& P, vec2& origin){
        glUseProgram(_pid_depth);
        glBindVertexArray(_vao_depth);
        {
            glActiveTexture(GL_TEXTURE8);
            glBindTexture(GL_TEXTURE_2D, _tex);
            /// @todo Bind paratemers
            glUniform1f(glGetUniformLocation(_pid_depth, "maxH"), maxHeight / displayRatio);
            glUniform1f(glGetUniformLocation(_pid_depth, "terR"), terrainRange / displayRatio);
            glUniform2fv(glGetUniformLocation(_pid_depth, "origin"), 1, origin.data());
            /// @todo Bind Matrices
            glUniformMatrix4fv(glGetUniformLocation(_pid_depth, "M"), 1, GL_FALSE, M.data());
            glUniformMatrix4fv(glGetUniformLocation(_pid_depth, "V"), 1, GL_FALSE, V.data());
            glUniformMatrix4fv(glGetUniformLocation(_pid_depth, "P"), 1, GL_FALSE, P.data());
            glDrawElements(GL_TRIANGLE_STRIP, _num_indices, GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);
        glUseProgram(0);
        check_error_gl();
    }
};

#endif

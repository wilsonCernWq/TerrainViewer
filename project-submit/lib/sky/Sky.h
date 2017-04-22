#ifndef CUBE_H
#define CUBE_H
#include "icg_common.h"
#include "../../src/icgfun.h"

namespace {
static const unsigned int nCubeVertices = 36;
static const vec3 cubeVertices[] =
{
    vec3(-0.5, -0.5, -0.5),
    vec3(-0.5, 0.5, -0.5),
    vec3(0.5, -0.5, -0.5),
    vec3(-0.5, 0.5, -0.5),
    vec3(0.5, -0.5, -0.5),
    vec3(0.5, 0.5, -0.5),
    vec3(0.5, 0.5, 0.5),
    vec3(0.5, -0.5, 0.5),
    vec3(0.5, 0.5, -0.5),
    vec3(0.5, -0.5, 0.5),
    vec3(0.5, 0.5, -0.5),
    vec3(0.5, -0.5, -0.5),
    vec3(0.5, 0.5, 0.5),
    vec3(-0.5, 0.5, 0.5),
    vec3(0.5, -0.5, 0.5),
    vec3(-0.5, 0.5, 0.5),
    vec3(0.5, -0.5, 0.5),
    vec3(-0.5, -0.5, 0.5),
    vec3(-0.5, -0.5, 0.5),
    vec3(-0.5, -0.5, -0.5),
    vec3(0.5, -0.5, 0.5),
    vec3(-0.5, -0.5, -0.5),
    vec3(0.5, -0.5, 0.5),
    vec3(0.5, -0.5, -0.5),
    vec3(-0.5, 0.5, -0.5),
    vec3(-0.5, -0.5, -0.5),
    vec3(-0.5, 0.5, 0.5),
    vec3(-0.5, -0.5, -0.5),
    vec3(-0.5, 0.5, 0.5),
    vec3(-0.5, -0.5, 0.5),
    vec3(0.5, 0.5, -0.5),
    vec3(-0.5, 0.5, -0.5),
    vec3(0.5, 0.5, 0.5),
    vec3(-0.5, 0.5, -0.5),
    vec3(0.5, 0.5, 0.5),
    vec3(-0.5, 0.5, 0.5)
};
}

class Sky{
private:
    typedef Eigen::Transform<float,3,Eigen::Affine> Transform;

protected:
    GLuint _vao; ///< vertex array object
    GLuint _pid; ///< GLSL shader program ID
    GLuint _vbo; ///< memory buffer

    GLuint _vaodepth; ///< vertex array object
    GLuint _piddepth; ///< GLSL shader program ID
    GLuint _vbodepth; ///< memory buffer

    GLuint _tex; ///< Texture ID
    mat4   _M;   ///< model matrix

public:
    /**
     * Destructor
     */
    ~Sky() { cleanup(); }

    /**
     * @brief init Initialize sky cube
     */
    void init(){
        /// @todo Compile the shaders
        _pid = opengp::load_shaders("lib/sky/sky_vshader.glsl", "lib/sky/sky_fshader.glsl");
        if(!_pid) { exit(EXIT_FAILURE); }
        glUseProgram(_pid);
        /// @todo Vertex one vertex Array
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
        /// @todo Vertex coordinates
        {
            //--- Buffer
            glGenBuffers(1, &_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBufferData(GL_ARRAY_BUFFER, nCubeVertices * sizeof(vec3), &cubeVertices[0], GL_STATIC_DRAW);

            //--- Attribute
            GLuint vpoint_id = glGetAttribLocation(_pid, "vpoint");
            glEnableVertexAttribArray(vpoint_id);
            glVertexAttribPointer(vpoint_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }
        /// @todo Load texture
        {
            glGenTextures(1, &_tex);
            glBindTexture(GL_TEXTURE_CUBE_MAP, _tex);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            int img_width = 512;
            int img_height = 512;
            /// @todo Positive X
            GLvoid* img;
            img = get_2dtga_pixel("lib/sky/sky_xp.tga", img_width, img_height, GL_RGB, GL_UNSIGNED_BYTE);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
            delete[] img;

            /// @todo Negative X
            img = get_2dtga_pixel("lib/sky/sky_xn.tga", img_width, img_height,GL_RGB, GL_UNSIGNED_BYTE);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
            delete[] img;

            /// @todo Positive Y
            img = get_2dtga_pixel("lib/sky/sky_yp.tga", img_width, img_height,GL_RGB, GL_UNSIGNED_BYTE);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
            delete[] img;

            /// @todo Negative Y
            img = get_2dtga_pixel("lib/sky/sky_yn.tga", img_width, img_height,GL_RGB, GL_UNSIGNED_BYTE);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
            delete[] img;

            /// @todo Positive Z
            img = get_2dtga_pixel("lib/sky/sky_zp.tga", img_width, img_height,GL_RGB, GL_UNSIGNED_BYTE);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
            delete[] img;

            /// @todo Negative Z
            img = get_2dtga_pixel("lib/sky/sky_zn.tga", img_width, img_height,GL_RGB, GL_UNSIGNED_BYTE);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
            delete[] img;
        }
        GLuint tex_id = glGetUniformLocation(_pid, "tex");
        glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);
        /// @todo to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
        check_error_gl();
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @todo Compile the shaders
        _piddepth = opengp::load_shaders("lib/sky/skyshadow_vshader.glsl", "lib/sky/skyshadow_fshader.glsl");
        if(!_piddepth) { exit(EXIT_FAILURE); }
        glUseProgram(_piddepth);
        /// @todo Vertex one vertex Array
        glGenVertexArrays(1, &_vaodepth);
        glBindVertexArray(_vaodepth);
        /// @todo Vertex coordinates
        {
            //--- Buffer
            glGenBuffers(1, &_vbodepth);
            glBindBuffer(GL_ARRAY_BUFFER, _vbodepth);
            glBufferData(GL_ARRAY_BUFFER, nCubeVertices * sizeof(vec3), &cubeVertices[0], GL_STATIC_DRAW);

            //--- Attribute
            GLuint vpoint_id = glGetAttribLocation(_piddepth, "vpoint");
            glEnableVertexAttribArray(vpoint_id);
            glVertexAttribPointer(vpoint_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }
        glBindVertexArray(0);
        glUseProgram(0);
        check_error_gl();
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @todo Create the model matrix
        Transform _M = Transform::Identity();
        _M *= Eigen::AngleAxisf(M_PI, Eigen::Vector3f::UnitZ());
        _M *= Eigen::AlignedScaling3f(100.0, 100.0, 100.0);
        _M *= Eigen::Translation3f(0,-0.3,0);
        this->_M = _M.matrix();
    }

    /**
     * @brief cleanup Clean object
     */
    void cleanup(){
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &_vbo);
        glDeleteProgram(_pid);
        glDeleteVertexArrays(1, &_vao);
        glDeleteTextures(1, &_tex);
    }

    /**
     * @brief draw
     * @param VP View * Projection
     * @param time current time
     */
    void draw(const mat4& VP, float time /*seconds*/){
        glUseProgram(_pid);
        glBindVertexArray(_vao);
        {
            check_error_gl();
            /// @todo Bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, _tex);
            /// @todo Time
            glUniform1f(glGetUniformLocation(_pid, "time"), time);
            check_error_gl();
            /// @todo Setup MVP
            mat4 MVP = VP * _M;
            GLuint MVP_id = glGetUniformLocation(_pid, "MVP");
            glUniformMatrix4fv(MVP_id, 1, GL_FALSE, MVP.data());
            ///--- Draw
            glDrawArrays(GL_TRIANGLES,0,nCubeVertices);
        }
        glBindVertexArray(0);
        glUseProgram(0);
        check_error_gl();
    }

    void drawfog(const mat4& VP, float time /*seconds*/){
        glUseProgram(_piddepth);
        glBindVertexArray(_vaodepth);
        {
            check_error_gl();
            /// @todo Time
            glUniform1f(glGetUniformLocation(_piddepth, "time"), time);
            check_error_gl();
            /// @todo Setup MVP
            mat4 MVP = VP * _M;
            GLuint MVP_id = glGetUniformLocation(_piddepth, "MVP");
            glUniformMatrix4fv(MVP_id, 1, GL_FALSE, MVP.data());
            ///--- Draw
            glDrawArrays(GL_TRIANGLES,0,nCubeVertices);
        }
        glBindVertexArray(0);
        glUseProgram(0);
        check_error_gl();
    }
};
#endif

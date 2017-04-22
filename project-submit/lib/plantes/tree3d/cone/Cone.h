#ifndef CONE_H
#define CONE_H
#include "icg_common.h"
#include <vector>

class Cone {
private:
    typedef Eigen::Transform<float,3,Eigen::Affine> Transform;
    const int CircleDivides = 100;
protected:
    GLuint _pid; //< GLSL shader program ID
    GLuint _vao; //< vertex array object
    GLuint _vbo0; //< memory buffer
    GLuint _vbo_index0; //< memory buffer
    GLuint _vbo1; //< memory buffer
    GLuint _vbo_index1; //< memory buffer
    GLuint _vbo2; //< memory buffer
    GLuint _vbo_index2; //< memory buffer
    GLuint _tex; //< Texture ID
    int _nvnormals;
    int _noindices;
    Transform _M;     //< model matrix
    
public:
    void setP(float x, float y, float z){
        _M *= Eigen::Translation3f(x, y, z);
    }

    void setR(float rx, float ry, float rz){
        _M *= Eigen::AngleAxisf(M_PI * rx, Eigen::Vector3f::UnitX());
        _M *= Eigen::AngleAxisf(M_PI * ry, Eigen::Vector3f::UnitY());
        _M *= Eigen::AngleAxisf(M_PI * rz, Eigen::Vector3f::UnitZ());
    }

    void setS(float scale = 1.0){
        _M *= Eigen::AlignedScaling3f(scale, scale, scale);
    }

    void init(float radius, float height){
        //--- Compile the shaders
        _pid = opengp::load_shaders("lib/plantes/tree3d/cone/cone_vshader.glsl", "lib/plantes/tree3d/cone/cone_fshader.glsl");
        if(!_pid) { exit(EXIT_FAILURE); }
        glUseProgram(_pid);
        
        //--- Vertex one vertex Array
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
     
        //--- Vertex coordinates
        std::vector<GLuint> indices;
        std::vector<GLfloat> vertices;
        std::vector<GLfloat> vnormals;
        std::vector<GLfloat> vtexture;

        {              
            float dh = height;
            float ds = sqrt(dh*dh + radius*radius);

            int num = (int)(radius * CircleDivides);
            float d = 360.0f / (float)num;

            //--> vertices
            //-- Alone border
            for (int i = 0; i < num; ++i) {
                float theta = M_PI * (float)i * d / 180.0f;
                float x = radius * cos(theta);
                float y = radius * sin(theta);
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(0.0);
            }
            //-- Peak
            vertices.push_back(0.0);
            vertices.push_back(0.0);
            vertices.push_back(height);


            //--> indices
            for (int i = 0; i < num; ++i) {
                indices.push_back(i);
                indices.push_back(num);
                indices.push_back((i+1)%num);
            }
            _noindices = indices.size();

            //--> vtexture
            float angle = 2.0f * M_PI * radius / ds;
            float dt = angle / (float)num;
            for (int i = 0; i < num; ++i) {
                float theta = M_PI * (float)i * dt / 180.0f;
                float x = 0.5 * cos(theta) + 0.5;
                float y = 0.5 * sin(theta) + 0.5;
                vtexture.push_back(x);
                vtexture.push_back(y);
            }
            vtexture.push_back(0.5);
            vtexture.push_back(0.5);

            //--> vnormal
            for (int i = 0; i <= num; ++i) {
                float theta = M_PI * (float)i * d / 180.0f;
                float x = abs(dh) * cos(theta) / ds;
                float y = abs(dh) * sin(theta) / ds;
                float z = dh * radius / ds / abs(dh);
                vnormals.push_back(x);
                vnormals.push_back(y);
                vnormals.push_back(z);
            }
            vnormals.push_back(0.0f);
            vnormals.push_back(0.0f);
            vnormals.push_back(dh / abs(dh));
        }

        {
            //-- vpoint buffer
            glGenBuffers(1, &_vbo0);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo0);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

            //-- vertex indices
            glGenBuffers(1, &_vbo_index0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo_index0);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

            //-- vpoint shader attribute
            GLuint vpoint_id = glGetAttribLocation(_pid, "vpoint");
            glEnableVertexAttribArray(vpoint_id);
            glVertexAttribPointer(vpoint_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            check_error_gl();
        }

        {
            //-- vnormal buffer
            glGenBuffers(1, &_vbo1);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo1);
            glBufferData(GL_ARRAY_BUFFER, vnormals.size() * sizeof(GLfloat), &vnormals[0], GL_STATIC_DRAW);

            //-- indices
            glGenBuffers(1, &_vbo_index1);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo_index1);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

            //-- vnormal shader attribute
            GLuint vpoint_id = glGetAttribLocation(_pid, "vnormal");
            glEnableVertexAttribArray(vpoint_id);
            glVertexAttribPointer(vpoint_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            check_error_gl();
        }

        {
            //-- vtexcoor buffer
            glGenBuffers(1, &_vbo2);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo2);
            glBufferData(GL_ARRAY_BUFFER, vtexture.size() * sizeof(GLfloat), &vtexture[0], GL_STATIC_DRAW);

            //-- indices
            glGenBuffers(1, &_vbo_index2);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo_index2);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

            //-- vtexcoor shader attribute
            GLuint vpoint_id = glGetAttribLocation(_pid, "vtexcoor");
            glEnableVertexAttribArray(vpoint_id);
            glVertexAttribPointer(vpoint_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            check_error_gl();
        }

        //--- Texture
        {
            //-- Generate Texture
            glGenTextures(1, &_tex);
            glBindTexture(GL_TEXTURE_2D, _tex);
            glfwLoadTexture2D("lib/plantes/tree3d/cone/Alternating_Mudbrick.tga", 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            //-- Texture uniforms
            GLuint tex_id = glGetUniformLocation(_pid, "tex");
            glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);
        }

        //--- to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
        
        //--- Initialize the model matrix (remember OpenGL is right handed)
        _M = Transform::Identity();
    }
    
    void cleanup(){
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &_vbo0);
        glDeleteBuffers(1, &_vbo_index0);
        glDeleteBuffers(1, &_vbo1);
        glDeleteBuffers(1, &_vbo_index1);
        glDeleteBuffers(1, &_vbo2);
        glDeleteBuffers(1, &_vbo_index2);
        glDeleteProgram(_pid);
        glDeleteVertexArrays(1, &_vao);
        glDeleteTextures(1, &_tex);
    }
    
    void draw(const mat4& VP, float time /*seconds*/){
        glUseProgram(_pid);
        glBindVertexArray(_vao);
        {
            //--- Bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _tex);

            //--- Time
            glUniform1f(glGetUniformLocation(_pid, "time"), time);
            //--- Setup MVP
            mat4 MVP = VP * _M.matrix();
            GLuint MVP_id = glGetUniformLocation(_pid, "MVP");
            glUniformMatrix4fv(MVP_id, 1, GL_FALSE, MVP.data());

            //--- Draw
            glDrawElements(GL_TRIANGLES, _noindices, GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);        
        glUseProgram(0);
    }
};

#endif

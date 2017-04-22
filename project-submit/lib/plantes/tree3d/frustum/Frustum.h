#ifndef FRUSTUM_H
#define FRUSTUM_H
#include "icg_common.h"
#include <vector>

class Frustum {
private:
    struct Light{
        vec3 Ia = vec3(1.0f, 1.0f, 1.0f);
        vec3 Id = vec3(1.0f, 1.0f, 1.0f);
        vec3 Is = vec3(1.0f, 1.0f, 1.0f);
        vec3 light_pos =  vec3(0.0f, 0.0f, 0.01f);

        //--- Pass light properties to the shader
        void setup(GLuint _pid){
            glUseProgram(_pid);
            GLuint light_pos_id = glGetUniformLocation(_pid, "Light"); //Given in camera space
            GLuint Ia_id = glGetUniformLocation(_pid, "Ia");
            GLuint Id_id = glGetUniformLocation(_pid, "Id");
            GLuint Is_id = glGetUniformLocation(_pid, "Is");
            glUniform3fv(light_pos_id, ONE, light_pos.data());
            glUniform3fv(Ia_id, ONE, Ia.data());
            glUniform3fv(Id_id, ONE, Id.data());
            glUniform3fv(Is_id, ONE, Is.data());
        }

        vec3 get_spot_direction(float time) {
            return light_pos;
        }
    };

    struct Material{
        vec3 ka = vec3(0.18f, 0.1f, 0.1f);
        vec3 kd = vec3(0.9f, 0.5f, 0.5f);
        vec3 ks = vec3(0.8f, 0.8f, 0.8f);
        float p = 60.0f;

        //--- Pass material properties to the shaders
        void setup(GLuint _pid){
            glUseProgram(_pid);
            GLuint ka_id = glGetUniformLocation(_pid, "ka");
            GLuint kd_id = glGetUniformLocation(_pid, "kd");
            GLuint ks_id = glGetUniformLocation(_pid, "ks");
            GLuint p_id = glGetUniformLocation(_pid, "p");
            glUniform3fv(ka_id, ONE, ka.data());
            glUniform3fv(kd_id, ONE, kd.data());
            glUniform3fv(ks_id, ONE, ks.data());
            glUniform1f(p_id, p);
        }
    };

    typedef Eigen::Transform<float,3,Eigen::Affine> Transform;
    const int CircleDivides = 500;
protected:
    GLuint _pid;        //< GLSL shader program ID
    GLuint _vao;        //< vertex array object
    GLuint _vbo0;       //< memory buffer
    GLuint _vbo_index0; //< memory buffer
    GLuint _vbo1;       //< memory buffer
    GLuint _vbo_index1; //< memory buffer
    GLuint _vbo2;       //< memory buffer
    GLuint _vbo_index2; //< memory buffer
    GLuint _tex;        //< Texture ID
    int _nvnormals;
    int _noindices;
    float _height;
    Transform _M;     //< model matrix
    Light _light;
    Material _material;
    
public:
    //--- Initialize the model matrix (remember OpenGL is right handed)
    Frustum(): _M(Transform::Identity()){}

    void aline(const Frustum& other) { _M = other._M; }

    void setT(float x, float y, float z){
        _M *= Eigen::Translation3f(x, y, z);
    }

    void setT(const vec3 pos){
        _M *= Eigen::Translation3f(pos);
    }

    void setR(vec3 n, float a){
        float az = atan2(n(1), n(0));
        float ay = atan2(n(2), sqrt(n(0)*n(0) + n(1)*n(1)));
        _M *= Eigen::AngleAxisf(-ay, Eigen::Vector3f::UnitY());
        _M *= Eigen::AngleAxisf(-az, Eigen::Vector3f::UnitZ());
        _M *= Eigen::AngleAxisf(a*M_PI/180.0, Eigen::Vector3f::UnitX());
        _M *= Eigen::AngleAxisf(az, Eigen::Vector3f::UnitZ());
        _M *= Eigen::AngleAxisf(ay, Eigen::Vector3f::UnitY());
    }

    void setR(float rx, float ry, float rz){
        _M *= Eigen::AngleAxisf(M_PI * rx, Eigen::Vector3f::UnitX());
        _M *= Eigen::AngleAxisf(M_PI * ry, Eigen::Vector3f::UnitY());
        _M *= Eigen::AngleAxisf(M_PI * rz, Eigen::Vector3f::UnitZ());
    }

    void setS(float scale = 1.0){
        _M *= Eigen::AlignedScaling3f(scale, scale, scale);
    }

    void init(float r1, float r2, float height){
        _height = height;
        //--- Compile the shaders
        _pid = opengp::load_shaders("lib/plantes/tree3d/frustum/frustum_vshader.glsl", "lib/plantes/tree3d/frustum/frustum_fshader.glsl");
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
            int num = (int)(r2 * CircleDivides);
            float d = 360.0f / (float)num;

            //--> indices
            for (int i = 0; i < num; ++i) {
                indices.push_back(i);
                indices.push_back((i+1)%num);
                indices.push_back(num + (i+1)%num);

                indices.push_back(i);
                indices.push_back(num + (i+1)%num);
                indices.push_back(num + i);
            }
            _noindices = indices.size();

            //--> vertices
            //-- upper border [0, num-1]
            for (int i = 0; i < num; ++i) {
                float theta = M_PI * (float)i * d / 180.0f;
                float x = r1 * cos(theta);
                float y = r1 * sin(theta);
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(height);
            }
            //-- lower border [num, 2*num-1]
            for (int i = 0; i < num; ++i) {
                float theta = M_PI * (float)i * d / 180.0f;
                float x = r2 * cos(theta);
                float y = r2 * sin(theta);
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(0.0f);
            }

            //--> vtexture
            //-- upper border
            for (int i = 0; i < num; ++i) {
                float theta = M_PI * (float)i * d / 180.0f;
                float u = 0.5 - 0.5 * cos(theta);
                float v = 1.0;
                vtexture.push_back(u);
                vtexture.push_back(v);
            }
            //-- lower border
            for (int i = 0; i < num; ++i) {
                float theta = M_PI * (float)i * d / 180.0f;
                float u = 0.5 - 0.5 * cos(theta);
                float v = 0.0;
                vtexture.push_back(u);
                vtexture.push_back(v);
            }

            //--> vnormal
            float hg = r1 * height / (r1 + r2);
            //-- upper border
            for (int i = 0; i <= num; ++i) {
                float theta = M_PI * (float)i * d / 180.0f;
                float x1 = r1 * cos(theta);
                float y1 = r1 * sin(theta);
                float z1 = r1 * (r2 - r1) / height;
                vec3 N = vec3(x1, y1, z1).normalized();
                vnormals.push_back(N(0));
                vnormals.push_back(N(1));
                vnormals.push_back(N(2));
            }
            // lower border
            for (int i = 0; i <= num; ++i) {
                float theta = M_PI * (float)i * d / 180.0f;
                float x2 = r2 * cos(theta);
                float y2 = r2 * sin(theta);
                float z2 = r2 * (r2 - r1) / height;
                vec3 N = vec3(x2, y2, z2).normalized();
                vnormals.push_back(N(0));
                vnormals.push_back(N(1));
                vnormals.push_back(N(2));
            }
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
            std::cout << vnormals.size() << "\n";
            std::cout << indices.size() << "\n";
            //-- vnormal shader attribute
            GLuint vnormal_id = glGetAttribLocation(_pid, "vnormal");
            if (vnormal_id >= 0) {
                glEnableVertexAttribArray(vnormal_id);
                glVertexAttribPointer(vnormal_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
                check_error_gl();
            }
            std::cout << vnormal_id << "\n";
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
            GLuint vtexcoor_id = glGetAttribLocation(_pid, "vtexcoor");
            glEnableVertexAttribArray(vtexcoor_id);
            glVertexAttribPointer(vtexcoor_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            check_error_gl();
        }

        //--- Texture
        {
            //-- Generate Texture
            glGenTextures(1, &_tex);
            glBindTexture(GL_TEXTURE_2D, _tex);
            glfwLoadTexture2D("src/Asylum_Cell_Orange.tga", 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            //-- Texture uniforms
            GLuint tex_id = glGetUniformLocation(_pid, "tex");
            glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);
        }

        //--- to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
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
    
    void draw(const mat4& View, const mat4& Project, float time /*seconds*/){
        glUseProgram(_pid);
        glBindVertexArray(_vao);
        _light.setup(_pid);
        _material.setup(_pid);
        {
            //--- Bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _tex);

            //--- Time
            glUniform1f(glGetUniformLocation(_pid, "time"), time);

            //--- Setup MVP
            GLuint Model_id = glGetUniformLocation(_pid, "Model");
            glUniformMatrix4fv(Model_id, 1, GL_FALSE, _M.matrix().data());

            GLuint View_id = glGetUniformLocation(_pid, "View");
            glUniformMatrix4fv(View_id, 1, GL_FALSE, View.data());

            GLuint Projection_id = glGetUniformLocation(_pid, "Projection");
            glUniformMatrix4fv(Projection_id, 1, GL_FALSE, Project.data());

            //--- Draw
            glDrawElements(GL_TRIANGLES, _noindices, GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);        
        glUseProgram(0);
    }

private:
    mat3 _rodrigue_matrix(const vec3& n, float alpha /* degree */){
        float cosa = cos(M_PI * alpha / 180.0);
        mat3 rom = mat3::Identity();
        mat3 N;
        N << 0.00f, +n(2), -n(1),
             -n(2), 0.00f, +n(0),
             +n(1), -n(0), 0.00f;
        rom *= cosa;
        rom += (1 - cosa) * n * n.transpose();
        rom += sqrt(1 - cosa * cosa) * N;
        return rom;
    }
};

#endif

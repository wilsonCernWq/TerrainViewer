#ifndef QUAD_H
#define QUAD_H
#include "icg_common.h"
#include <vector>
#include <string>

/** Basic shape object */
class Quad{
protected:
    GLuint _pid;  ///< shader program ID
    GLuint _vao;  ///< vertex array object
    GLuint _tex;  ///< Texture Id
    string _vshader;  ///< v-shader filename
    string _fshader;  ///< f-shader filename
    string _texfile;  ///< texture filename
    std::vector<GLuint> _buffer_objects;  ///< memory buffers
    
public:
    /**
     * @brief Quad: Constructor
     */
    Quad() {}

    /**
      * @brief ~Quad: Destructor
      */
    ~Quad() { cleanup(); }

protected:
    /**
     * @brief genBuffer: Generate a new buffer and return its Id
     * @param n: Number of layer for the new buffer
     * @return New buffer Id
     */
    GLuint genBuffer(GLsizei n = 1) {
        /// @todo Generate a n layer buffer
        GLuint buffer;
        glGenBuffers(n, &buffer);
        /// @todo Store buffer into vector
        _buffer_objects.push_back(buffer);
        return buffer;
    }

    /**
     * @brief delBuffer: Delete all the created buffers
     */
    void delBuffer() {
        typedef std::vector<GLuint>::iterator IT;
        for (IT it = _buffer_objects.begin(); it != _buffer_objects.end(); ++it){
            glDeleteBuffers(1, &(*it));
        }
    }

protected:
    /**
     * @brief ShaderParameters: Overridable function for initialize shader inputs
     */
    virtual void ShaderParameters(GLuint program){
        /// @todo Set vpoint(GLfloat): Vertex coordinates
        {
            const GLfloat vpoint[] = { /*V1*/ -1.0f, -1.0f, 0.0f,
                                       /*V2*/ +1.0f, -1.0f, 0.0f,
                                       /*V3*/ -1.0f, +1.0f, 0.0f,
                                       /*V4*/ +1.0f, +1.0f, 0.0f };
            /// @todo Create buffer & load data
            glBindBuffer(GL_ARRAY_BUFFER, genBuffer());
            glBufferData(GL_ARRAY_BUFFER, sizeof(vpoint), vpoint, GL_STATIC_DRAW);

            /// @todo Set parameter Attributes
            GLuint id = glGetAttribLocation(program, "vpoint");
            glEnableVertexAttribArray(id);
            glVertexAttribPointer(id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            check_error_gl();
        }

        /// @todo Set vpoint(GLfloat): Texture coordinates
        {
            const GLfloat vtexcoord[] = { /*V1*/ 0.0f, 0.0f,
                                          /*V2*/ 1.0f, 0.0f,
                                          /*V3*/ 0.0f, 1.0f,
                                          /*V4*/ 1.0f, 1.0f};
            /// @todo: Create buffer & load data
            glBindBuffer(GL_ARRAY_BUFFER, genBuffer());
            glBufferData(GL_ARRAY_BUFFER, sizeof(vtexcoord), vtexcoord, GL_STATIC_DRAW);
            check_error_gl();
            /// @todo Set parameter Attributes
            GLuint id = glGetAttribLocation(program, "vtexcoord");
            glEnableVertexAttribArray(id);
            check_error_gl();
            glVertexAttribPointer(id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            check_error_gl();
        }
    }

    /**
     * @brief ShaderTexture: Overridable function for initialize default texture
     */
    virtual void ShaderTexture(GLuint program){
        /// @todo Load texture
        glGenTextures(1, &_tex);
        glBindTexture(GL_TEXTURE_2D, _tex);
        glfwLoadTexture2D(_texfile.c_str(), 0);

        /// @todo Set texture paratemers
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        /// @todo Pass texture as uniforms
        GLuint id = glGetUniformLocation(program, "tex");
        glUniform1i(id, 0 /* GL_TEXTURE0 */);
    }

    virtual void InitMembers() {}

public:
    /**
     * @brief setShader: set shader file
     * @param vshader: v-shader filename
     * @param fshader: f-shader filename
     */
    void setShader(const char* vshader, const char* fshader){
        _vshader = string(vshader);
        _fshader = string(fshader);
    }

    /**
     * @brief setTexture: set texture filename
     * @param texture: texture filename
     */
    void setTexture(const char* texture){ _texfile = string(texture); }

    /**
     * @brief cleanup: function to cleanup object
     */
    void cleanup(){
        glBindVertexArray(0);
        glUseProgram(0);
        delBuffer();
        glDeleteTextures(1, &_tex);
        glDeleteVertexArrays(1, &_vao);
        glDeleteProgram(_pid);
    }

public:
    /**
     * @brief init: Overridable function for initialize object
     */
    virtual void init(bool multiple_initialization = false){
        if (!multiple_initialization){
            /// @todo Compile the shaders
            _pid = opengp::load_shaders(_vshader.c_str(), _fshader.c_str());
            if(!_pid) { exit(EXIT_FAILURE); }
        }

        /// @todo Initialize shaders
        glUseProgram(_pid);
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
        ShaderParameters(_pid);
        ShaderTexture(_pid);

        /// @todo To avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
        check_error_gl();

        /// @todo Initialize other parts
        InitMembers();
        check_error_gl();
    }
    
    /**
     * @brief draw: Overridable function for object drawing
     * @param MVP: 3D transformation matrix
     */
    void draw(const mat4& MVP){
        glUseProgram(_pid);
        glBindVertexArray(_vao);

        /// @todo Drawing setup
        {
            /// @todo Bind textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _tex);
            
            /// @todo Setup MVP
            GLuint MVP_id = glGetUniformLocation(_pid, "MVP");
            glUniformMatrix4fv(MVP_id, 1, GL_FALSE, MVP.data());   
            
            /// @todo Draw
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        glBindVertexArray(0);
        glUseProgram(0);
        check_error_gl();
    }
};

#endif

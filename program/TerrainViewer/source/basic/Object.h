#ifndef _OBJECT_H_
#define _OBJECT_H_
#include <map>
#include <vector>
#include <string>
#include "../icg_helper.h"

/** Basic shape object */
class Object{
private:
    static std::map<std::string, GLuint> _TEXTURE_LIST_;
    static std::map<std::string, GLuint> _TEXTURE_SIZE_;
    static GLuint _TEXTURE_OFFSET_;
private:
    typedef std::vector<GLuint>::iterator _List_It_;
    std::vector<GLuint> _bufferV_list_; ///< buffer value list
    std::vector<GLuint> _bufferN_list_; ///< buffer name list
    std::vector<GLuint> _program_list_; ///< program list
    std::vector<GLuint> _vertexA_list_; ///< vertex array object list

protected:

    void SetShader1i(const GLuint& program, const char* var, const int& value)
    {
        glUniform1i(glGetUniformLocation(program, var), value);
    }

    void SetShader1f(const GLuint& program, const char* var, const float& value)
    {
        glUniform1f(glGetUniformLocation(program, var), value);
    }

    void SetShader2v(const GLuint& program, const char* var, const vec2& value)
    {
        glUniform2fv(glGetUniformLocation(program, var), ONE, value.data());
    }

    void SetShader3v(const GLuint& program, const char* var, const vec3& value)
    {
        glUniform3fv(glGetUniformLocation(program, var), ONE, value.data());
    }

    void SetShader4m(const GLuint& program, const char* var, const mat4& value)
    {
        glUniformMatrix4fv(glGetUniformLocation(program, var), ONE, GL_FALSE, value.data());
    }

    void SetShaderTexture2D(const GLuint& program, const std::string ref, const char* var)
    {
        ///--- Throw error if the key is already exist
        if (_TEXTURE_LIST_.find(ref) != _TEXTURE_LIST_.end())
        {
            std::cerr << "ERROR: Texutre Reference Redefintion: " << ref << endl;
            exit(EXIT_FAILURE);
        }
        ///--- Otherwise insert new element
        _TEXTURE_LIST_[ref] = _TEXTURE_OFFSET_;
        _TEXTURE_SIZE_[ref] = 1;
        ++_TEXTURE_OFFSET_;
        ///--- Bind texture to program variable
        glUniform1i(glGetUniformLocation(program, var), _TEXTURE_LIST_[ref]);
    }

    void SetShaderTexture2Ds(const GLuint& program, const std::string ref, char** var, GLuint num)
    {
        ///--- Throw error if the key is already exist
        if (_TEXTURE_LIST_.find(ref) != _TEXTURE_LIST_.end())
        {
            std::cerr << "ERROR: Texutre Reference Redefintion: " << ref << endl;
            exit(EXIT_FAILURE);
        }
        ///--- Otherwise insert new element
        _TEXTURE_LIST_[ref] = _TEXTURE_OFFSET_;
        _TEXTURE_SIZE_[ref] = num;
        _TEXTURE_OFFSET_   += num;
        ///--- Bind texture to program variable
        for (GLuint i = 0; i < num; ++i)
        {
            glUniform1i(glGetUniformLocation(program, var[i]), _TEXTURE_LIST_[ref] + i);
        }
    }


    void ActiveShaderTexture2D(const std::string ref, const GLuint* tex) {
        ///--- Throw error if the key is not exist
        if (_TEXTURE_LIST_.find(ref) == _TEXTURE_LIST_.end())
        {
            std::cerr << "ERROR: Texutre Reference Not Found: " << ref << endl;
            exit(EXIT_FAILURE);
        }
        ///--- Otherwise Active Texture
        for (GLuint i = 0; i < _TEXTURE_SIZE_[ref]; ++i)
        {
            glActiveTexture(GL_TEXTURE0 + _TEXTURE_LIST_[ref] + i);
            glBindTexture(GL_TEXTURE_2D, tex[i]);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////
    /**
     * @brief genBuffer: Generate a new buffer and return its Id
     * @param n: Number of layer for the new buffer
     * @return New buffer Id
     */
    GLuint genBuffer(GLsizei n = 1)
    {
        ///--- Generate a n layer buffer
        GLuint buffer;
        glGenBuffers(n, &buffer);
        ///--- Store buffer into vector
        _bufferV_list_.push_back(buffer);
        _bufferN_list_.push_back(n);
        return buffer;
    }
    /**
     * @brief delBuffer: Delete all the created buffers
     */
    void delBuffer()
    {
        for (int i = 0; i < _bufferV_list_.size(); ++i)
        {
            glDeleteBuffers(_bufferN_list_[i], &(_bufferV_list_[i]));
        }
        _bufferN_list_.clear();
        _bufferV_list_.clear();
    }
    /**
     * @brief genProgram: Generate new program
     * @param vshader: vertex shader location
     * @param fshader: fragment shader location
     * @return program index
     */
    GLuint genProgram(char* vshader, char* fshader, char* gshader = NULL)
    {
        GLuint program_id = opengp::load_shaders(vshader, fshader, gshader);
        GLuint vertexA_id;
        if(!program_id) { exit(EXIT_FAILURE); }
        glGenVertexArrays(1, &vertexA_id);
        _program_list_.push_back(program_id);
        _vertexA_list_.push_back(vertexA_id);
        return program_id;
    }
    /**
     * @brief delProgram: delete all the programs
     */
    void delProgram()
    {
        for (int i = 0; i < _program_list_.size(); ++i)
        {
            glDeleteVertexArrays(1, &(_vertexA_list_[i]));
            glDeleteProgram(_program_list_[i]);
        }
        _vertexA_list_.clear();
        _program_list_.clear();
    }
    /**
     * @brief getProgramId: Get ith program index
     * @param i
     * @return
     */
    GLuint getProgramId(GLuint i = 0){ return _program_list_[i]; }
    /**
     * @brief bindProgram: Bind ith program
     * @param i
     */
    void bindProgram(GLuint i = 0)
    {
        glUseProgram(_program_list_[i]);
        glBindVertexArray(_vertexA_list_[i]);
    }
    /**
     * @brief unbindProgram: unbind current program
     */
    void unbindProgram()
    {
        glBindVertexArray(0);
        glUseProgram(0);
    }
    /**
     * @brief cleanup: function to cleanup object
     */
    virtual void cleanup()
    {
        _TEXTURE_LIST_.clear();
        _TEXTURE_SIZE_.clear();
        unbindProgram();
        delBuffer();
        delProgram();
    }
public:
    /**
     * @brief Quad: Constructor
     */
    Object() {}
    /**
      * @brief ~Quad: Destructor
      */
    virtual ~Object() { cleanup(); }
};

std::map<std::string, GLuint> Object::_TEXTURE_LIST_;
std::map<std::string, GLuint> Object::_TEXTURE_SIZE_;
GLuint Object::_TEXTURE_OFFSET_ = 0;

#endif // _OBJECT_H_

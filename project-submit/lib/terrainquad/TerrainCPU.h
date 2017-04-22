#ifndef TERRAINCPU_H_
#define TERRAINCPU_H_
#include "icg_common.h"
#include "../quad/Quad.h"
#include "../Noise.h"

class TerrainCPU: public Quad {
protected:
    float _H = 0.9;
    float _L = 4.7;
    float _O = 8.0;
    float _offset = 0.9;
    float _gain = 4.25;
    GLfloat& terrainRange; ///< XY coordinate spaining range
protected:
    float _max;
    float _min;
    GLfloat* _vertices;
    GLuint _num_vertices;  ///< number of vertices to render
    GLuint* _indices;
    GLuint _num_indices;  ///< number of vertices to render
public:
    TerrainCPU(GLfloat& range):terrainRange(range){
        _vshader = "lib/terrainquad/terraincpu_vshader.glsl";
        _fshader = "lib/terrainquad/terraincpu_fshader.glsl";
    }

    void ShaderTexture(GLuint) {}

    void ShaderParameters(GLuint program){
        int dim = 1024;  ///< grid dimension
        Noise noise = Noise(_H, _L, _O);

        /// @todo Setup vertices position
        {
            /// @todo To allocate vertices array
            _num_vertices = 3 * (dim + 1) * (dim + 1);
            _vertices = new GLfloat[_num_vertices];

            /// @todo To calculate XYZ coordinate for each point
            int k = 0;
            const float h = 2.0f / dim;  ///< increment distance
            for (int i = 0; i < dim + 1; ++i){
                for (int j = 0; j < dim + 1; ++j){
                    /// @todo calculate XY
                    _vertices[k++] = -1.0f + i * h;
                    _vertices[k++] = -1.0f + j * h;

                    /// @todo calculate Z
                    float x = 2.0f * i * terrainRange / dim;
                    float y = 2.0f * j * terrainRange / dim;
                    float z = noise(x, y, 0.0, _offset, _gain);

                    /// @todo calculate Zmax & Zmin
                    if(!i && !j) { _max = z; _min = z; }
                    _max = z > _max ? z : _max;
                    _min = z < _min ? z : _min;

                    /// @todo save Z value
                    _vertices[k++] = z;
                }
            }
        }

        /// @todo Setup vertices indices
        {
            /// @todo To allocate indices array
            _num_indices = 2 * (dim + 1) * (dim + 2);
            _indices = new GLuint[_num_indices];

            /// @todo To compute indices array
            int k = 0;
            for (int i = 0; i < dim; ++i){
                for (int j = 0; j < dim + 1; ++j){
                    _indices[k++] = (j + (dim + 1) * i);
                    _indices[k++] = (j + (dim + 1) * (i + 1));
                }
                _indices[k++] = ((dim + 1) * (i + 1) + dim);
                _indices[k++] = ((dim + 1) * (i + 1));
            }
        }

        /// @todo Setup position buffer
        glBindBuffer(GL_ARRAY_BUFFER, genBuffer());
        glBufferData(GL_ARRAY_BUFFER, _num_vertices * sizeof(GLfloat), &_vertices[0], GL_DYNAMIC_DRAW);

        /// @todo Setup vertex indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, genBuffer());
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _num_indices * sizeof(GLuint), &_indices[0], GL_DYNAMIC_DRAW);

        /// @todo Setup position shader attribute
        GLuint id = glGetAttribLocation(program, "vpoint");
        glEnableVertexAttribArray(id);
        glVertexAttribPointer(id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        check_error_gl();
    }

    void draw(){
        glUseProgram(_pid);
        glBindVertexArray(_vao);
        {
            glUniform1f(glGetUniformLocation(_pid, "max"), _max);
            glUniform1f(glGetUniformLocation(_pid, "min"), _min);
            glDrawElements(GL_TRIANGLE_STRIP, _num_indices, GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);
        glUseProgram(0);
        check_error_gl();
    }
};

#endif

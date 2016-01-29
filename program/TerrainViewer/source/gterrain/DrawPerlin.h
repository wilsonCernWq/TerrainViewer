#ifndef _DRAWPERLIN_H_
#define _DRAWPERLIN_H_

#include "../icg_helper.h"
#include "../framebuffer/FrameBuffer.h"
#include "../basic/Object.h"
#include "../perlin/perlin.h"

class DrawPerlin: public Object {
private:
    GLuint  resolution; ///< mesh dimension for CPU calculation
    GLfloat span;       ///< noise spaning range
    GLfloat Zmax = 0.0f;  ///< noise maximum value
    GLfloat Zmin = 0.0f;  ///< noise minimum value
    PerlinNoise perlin;
private:
    GLuint pid_gpu;  ///< program id
    GLuint pid_cpu;  ///< program id
    GLuint numV_cpu; ///< vertex list size
    GLuint numF_cpu; ///< face indices number
    vector<GLfloat> VList_cpu; ///< vertex list
    vector<GLuint>  FList_cpu; ///< face indices list
private:
    FrameBuffer texfb;
    GLint iFormat;
    GLint oFormat;
public:
    GLuint tex;
public:
    ///--- Constructor
    DrawPerlin(GLuint  r = 1024,
               GLfloat s = 64.0f,
               GLint internalFormat = GL_R8,
               GLint Format = GL_RED):
        resolution(r),
        span(s),
        iFormat(internalFormat),
        oFormat(Format),
        texfb(FrameBuffer(r, r))
    {}
    ///--- Destructor
    ~DrawPerlin() { cleanup(); }

    ///--- Clean up data
    void cleanup()
    {
        texfb.cleanup();
        Object::cleanup();
    }

    ///--- Initialization
    GLuint init()
    {
        ///--- CPU generation
        pid_cpu = genProgram("drawperlin_cpu_technique_vs.glsl",
                             "drawperlin_cpu_technique_fs.glsl");
        bindProgram(0);
        {
            ///--- To computer vertices position
            for (int i = 0; i < resolution + 1; ++i){
            for (int j = 0; j < resolution + 1; ++j){
                float x = span * i / resolution;
                float y = span * j / resolution;
                float z = perlin.noise(x, y, 0.0);
                VList_cpu.push_back(-1.0f + 2.0f * i / resolution);
                VList_cpu.push_back(-1.0f + 2.0f * j / resolution);
                VList_cpu.push_back(z);
                Zmax = max(Zmax, z);
                Zmin = min(Zmin, z);
            }
            }
            ///--- To compute face indices
            for (int i = 0; i < resolution    ; ++i){
            for (int j = 0; j < resolution + 1; ++j){
                FList_cpu.push_back(j + (resolution + 1) *  i);
                FList_cpu.push_back(j + (resolution + 1) * (i + 1));
            }
            if (i == resolution) { break; }
            FList_cpu.push_back((resolution + 1) * (i + 1) + resolution);
            FList_cpu.push_back((resolution + 1) * (i + 1));
            }
            ///--- Setup vertices and faces buffer
            glBindBuffer(GL_ARRAY_BUFFER, genBuffer());
            glBufferData(GL_ARRAY_BUFFER, VList_cpu.size() * sizeof(GLfloat), &VList_cpu[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, genBuffer());
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, FList_cpu.size() * sizeof(GLuint), &FList_cpu[0], GL_STATIC_DRAW);
            GLuint id = glGetAttribLocation(pid_cpu, "vpoint");
            glEnableVertexAttribArray(id);
            glVertexAttribPointer(id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }
        unbindProgram();
        ///--- GPU generation
        pid_gpu = genProgram("drawperlin_gpu_technique_vs.glsl",
                             "drawperlin_gpu_technique_fs.glsl");
        bindProgram(1);
        {
            const GLfloat vpoint[] = { /*V1*/ -1.0f, -1.0f,
                                       /*V2*/ +1.0f, -1.0f,
                                       /*V3*/ -1.0f, +1.0f,
                                       /*V4*/ +1.0f, +1.0f };
            glBindBuffer(GL_ARRAY_BUFFER, genBuffer());
            glBufferData(GL_ARRAY_BUFFER, sizeof(vpoint), vpoint, GL_STATIC_DRAW);
            GLuint id = glGetAttribLocation(pid_gpu, "vpoint");
            glEnableVertexAttribArray(id);
            glVertexAttribPointer(id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }
        unbindProgram();
        ///--- Error check
        tex = texfb.initColor(1, iFormat, oFormat)[0];
        check_error_gl();
        return tex;
    }

    ///--- Draw on texture
    void draw(GLuint technique = 1){
        if (technique == 0) {
            bindProgram(0);
            SetShader1f(pid_cpu, "max", Zmax);
            SetShader1f(pid_cpu, "min", Zmin);
            glDrawElements(GL_TRIANGLE_STRIP, FList_cpu.size()*sizeof(GLuint), GL_UNSIGNED_INT, 0);
        } else if (technique == 1) {
            bindProgram(1);
            SetShader1f(pid_gpu, "span", span);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        } else {
            std::cerr << "ERROR: Unknown Technique" << endl;
        }
        unbindProgram();
        check_error_gl();
    }

    ///--- Draw noise on a framebuffer
    void drawTexture() {
        texfb.bind(0);
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            this->draw();
        }
        texfb.unbind();
    }
};

#endif // _DRAWPERLIN_H_

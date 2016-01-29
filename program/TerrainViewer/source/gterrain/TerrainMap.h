#ifndef _THEIGHT_H_
#define _THEIGHT_H_
#include "../icg_helper.h"
#include "../framebuffer/FrameBuffer.h"
#include "../basic/Object.h"
#include "DrawPerlin.h"

/**
 * @brief The TerrainMap class Draw Height Map using MultiFractal
 */
class TerrainMap: public Object {
private:
    GLuint _pid_;
    GLuint _ref_;
    GLfloat* _H_; ///< lambda
    GLfloat* _L_; ///< lacunarity
    GLfloat* _O_; ///< octaves
    GLfloat* _G_; ///< gain
    GLfloat* _B_; ///< offset
    GLfloat  _P_; ///< persistance
    vec2& _origin_;
private:
    FrameBuffer _texfb_;
    GLint iFormat;
    GLint oFormat;
public:
    GLuint tex;
public:
    TerrainMap(GLuint imageSize,
               GLint internalFormat,
               GLint Format,
               vec2& origin):
        _texfb_(FrameBuffer(imageSize,imageSize)),
        _origin_(origin),
        iFormat(internalFormat = GL_RGB32F),
        oFormat(Format = GL_RGB)
    {}

    /**
     * @brief TerrainHeight Constructor
     * @param lambda
     * @param lacunarity
     * @param octaves
     * @param gain
     * @param offset
     * @param center
     */
    void Parameters(GLfloat& lambda,
                    GLfloat& lacunarity,
                    GLfloat& octaves,
                    GLfloat& gain,
                    GLfloat& offset)
    {
        _H_ = &lambda;
        _L_ = &lacunarity;
        _O_ = &octaves;
        _G_ = &gain;
        _B_ = &offset;
    }

    /**
     * @brief init Initialization
     */
    GLuint init()
    {       
        tex = _texfb_.initColor(1, iFormat, oFormat)[0];
        ///--- Initialize program
        _pid_ = genProgram("rmfractal_vshader.glsl", "rmfractal_fshader.glsl");
        bindProgram();
        SetupQuad();
        unbindProgram();
        check_error_gl();
        return tex;
    }

    /**
     * @brief drawTexture Draw Perlin Noise into texture
     */
    void setTexture(GLuint texture) { _ref_ = texture; }

    /**
     * @brief SetupQuad: Initialize shader inputs
     */
    void SetupQuad()
    {
        SetShaderTexture2D(_pid_, "mapNoisePerlin", "reference");
        const GLfloat vpoint[] = { /*V1*/ -1.0f, -1.0f,
                                   /*V2*/ +1.0f, -1.0f,
                                   /*V3*/ -1.0f, +1.0f,
                                   /*V4*/ +1.0f, +1.0f };
        glBindBuffer(GL_ARRAY_BUFFER, genBuffer());
        glBufferData(GL_ARRAY_BUFFER, sizeof(vpoint), vpoint, GL_STATIC_DRAW);
        GLuint id = glGetAttribLocation(_pid_, "vpoint");
        glEnableVertexAttribArray(id);
        glVertexAttribPointer(id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        check_error_gl();
    }

    /**
     * @brief draw Draw texture by Ridged Multifractal
     */
    void draw()
    {
        _P_ = pow(*_L_, *_H_);
        bindProgram();
        {
            ActiveShaderTexture2D("mapNoisePerlin", &_ref_);
            SetShader1f(_pid_, "P",  _P_);
            SetShader1f(_pid_, "L", *_L_);
            SetShader1f(_pid_, "O", *_O_);
            SetShader1f(_pid_, "G", *_G_);
            SetShader1f(_pid_, "B", *_B_);
            SetShader2v(_pid_, "origin", _origin_);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        unbindProgram();
        check_error_gl();
    }

    ///--- Draw noise on a framebuffer
    void drawTexture() {
        _texfb_.bind(0);
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            this->draw();
        }
        _texfb_.unbind();
    }
};

#endif // _THEIGHT_H_

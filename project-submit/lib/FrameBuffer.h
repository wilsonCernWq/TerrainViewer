#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_
#include "icg_common.h"

/// @remarks Create only one renderbuffer
class FrameBuffer{
protected:
    /// @name fbo contents
    GLuint _fbo;  ///< framebuffer object
    GLuint _render_tex;  ///< depth texture
    GLuint* _image_tex;  ///< color attachments
    GLsizei _image_num;  ///< image layer number
    /// @name Constants
    int _imgwidth;
    int _imgheight;
    /// @name Mode
    bool _init = false;
    bool _depthtex_mode = false;
    bool _interpolation = true;
    /// @name format
    GLenum _format = GL_RGB;
    GLenum _internal_format = GL_RGB32F;
    /// @name records
    GLint _previewport[4];

public:
    /**
     * @brief FrameBuffer: Constructor
     * @param image_width
     * @param image_height
     */
    FrameBuffer(int image_width, int image_height):
        _imgwidth(image_width), _imgheight(image_height){}

    /**
      * @brief ~FrameBuffer: Destructor
      */
    ~FrameBuffer() { cleanup(false); }

    /**
     * @brief bind: bind buffer to draw
     * @param n: starting buffer index
     * @param len: number of buffer to be drawn
     */
    void bind(int n = 0, int len = 0) {
        /// @todo Check initialization
        if (!_init) {
            std::cerr << "ERROR:: Framebuffer is not initialized!" << std::endl;
            exit(EXIT_FAILURE);
        }
        /// @todo Ovverrides Viewport
        glGetIntegerv(GL_VIEWPORT, _previewport);
        glViewport(0, 0, _imgwidth, _imgheight);
        /// @todo Setup buffer target
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        if (!_depthtex_mode && len == 0) {
            /// @todo Draw single buffer
            glDrawBuffer(GL_COLOR_ATTACHMENT0 + n);
        } else if (!_depthtex_mode && len != 0){
            /// @todo Draw Multiple Buffers
            GLenum* buffers = new GLenum[len];
            for (int i = 0; i <= len; ++i){ buffers[i] = GL_COLOR_ATTACHMENT0+i+n; }
            glDrawBuffers(len /*length of buffers[]*/, buffers);
        }
    }


    /**
     * @brief unbind: unbind all textures from current buffer
     */
    void unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(_previewport[0],_previewport[1],_previewport[2],_previewport[3]);
    }

    /**
     * @brief setForamt Set color texture format
     * @param format
     * @param internalformat
     */
    void setForamt(GLenum format,  GLenum internalformat) {
        _format = format; _internal_format = internalformat;
    }

    /**
     * @brief init: initialize frame buffer object
     * @param n: number of color texture to be created
     * @param use_interpolation
     * @return fbo parameter
     */
    GLuint* init(int n = 1, bool interpolation = true) {
        _init = true;
        _interpolation = interpolation;
        //////////////////////////////////////////////////////////////////////////////
        /// @todo Create frame buffer object
        //////////////////////////////////////////////////////////////////////////////
        glGenFramebuffers(1, &_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        if (n>0) {
            //////////////////////////////////////////////////////////////////////////
            /// @todo Create color attachments
            //////////////////////////////////////////////////////////////////////////
            _image_tex = new GLuint[n];
            _image_num = n;
            glGenTextures(n, _image_tex);
            for (int i = 0; i < n; ++i){
                glBindTexture(GL_TEXTURE_2D, _image_tex[i]);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
                if(interpolation){
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                } else {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                }
            
                /// @todo Create texture for the color attachment
                glTexImage2D(GL_TEXTURE_2D, 0 /* mid-map level */,
                             _internal_format /* internalFormat */,
                             _imgwidth, _imgheight, 0, _format /* format */,
                             GL_UNSIGNED_BYTE, NULL /* empty buffer */);

                /// @todo Tie texture to frame buffer object
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                                       GL_TEXTURE_2D, _image_tex[i], 0 /* level */);
            }
            glBindTexture(GL_TEXTURE_2D, 0);  ///< Avoid pollution

            //////////////////////////////////////////////////////////////////////////
            /// @todo Create render buffer
            //////////////////////////////////////////////////////////////////////////
            glGenRenderbuffers(1, &_render_tex);
            glBindRenderbuffer(GL_RENDERBUFFER, _render_tex);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
                                  _imgwidth, _imgheight);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                      GL_RENDERBUFFER, _render_tex);
            glBindRenderbuffer(GL_RENDERBUFFER, 0); ///< Avoid pollution

        } else {
            _depthtex_mode = true;
            //////////////////////////////////////////////////////////////////////////
            /// @todo Create color attachments
            //////////////////////////////////////////////////////////////////////////
            _image_tex = new GLuint[1];
            _image_num = 1;
            glGenTextures(1, _image_tex);
            glBindTexture(GL_TEXTURE_2D, _image_tex[0]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0 /* mid-map level */,
                         GL_DEPTH_COMPONENT24 /* internalFormat */,
                         _imgwidth, _imgheight, 0, GL_DEPTH_COMPONENT,
                         GL_FLOAT, NULL /* empty buffer */);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                 _image_tex[0], 0 /* level */);
            glDrawBuffer(GL_NONE);
        }

        //////////////////////////////////////////////////////////////////////////////
        /// @todo Check frame buffer status
        //////////////////////////////////////////////////////////////////////////////
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "!!!ERROR: Framebuffer is not complete!" << std::endl;
            exit(EXIT_FAILURE);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0); ///< avoid pollution
        check_error_gl();
        return _image_tex;
    }

    /**
     * @brief cleanup clean all buffers
     */
    void cleanup(bool clean_tex = false) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0 /*UNBIND*/);
        glDeleteFramebuffers(1, &_fbo);
        glDeleteRenderbuffers(1, &_render_tex);
        if (clean_tex) { glDeleteTextures(_image_num, _image_tex); }
    }
};
#endif

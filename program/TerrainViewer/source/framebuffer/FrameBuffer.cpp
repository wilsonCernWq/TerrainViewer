#include "FrameBuffer.h"

/**
  * @brief bind: bind buffer to draw
  * @param n: starting buffer index
  * @param len: number of buffer to be drawn
  */
void FrameBuffer::bind(int offset, int len)
{
    glGetIntegerv(GL_VIEWPORT, _previewport_);
    glViewport(0, 0, _imgW_, _imgH_);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo_);
    ///--- Bind color attachments
    if (mode == COLOR)
    {
        _color_attachments_ = new GLenum[len+1];
        for (int i = 0; i <= len; ++i)
        {
            _color_attachments_[i] = GL_COLOR_ATTACHMENT0 + i + offset;
        }
        glDrawBuffers(len+1 /*length of buffers[]*/, _color_attachments_);
    }
}

/**
  * @brief unbind: unbind all textures from current buffer
  */
void FrameBuffer::unbind()
{
    delete[] _color_attachments_;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(_previewport_[0],
               _previewport_[1],
               _previewport_[2],
               _previewport_[3]);
}

/**
  * @brief init: initialize n color layer
  * @param n: number of color texture to be created
  * @return color texture list
  */
GLuint* FrameBuffer::initColor(int n, GLint internalFormat, GLint Format)
{
    mode = COLOR;
    _layer_number_ = n;
    ///--- Create frame buffer object
    glGenFramebuffers(1, &_fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo_);
    ///--- Create color attachments
    _return_image_ = new GLuint[n];
    glGenTextures(n, _return_image_);
    for (int i = 0; i < n; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, _return_image_[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _imgW_, _imgH_, 0, Format, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,GL_TEXTURE_2D, _return_image_[i], 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    ///--- Create render buffer
    glGenRenderbuffers(1, &_depth_buffer_);
    glBindRenderbuffer(GL_RENDERBUFFER, _depth_buffer_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _imgW_, _imgH_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depth_buffer_);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    check();
    return _return_image_;
}

/**
 * @brief FrameBuffer::initDepth Initialize only one Depth Buffer
 * @return
 */
GLuint* FrameBuffer::initDepth()
{
    mode = DEPTH;
    _layer_number_ = 1;
    ///--- Create frame buffer object
    glGenFramebuffers(1, &_fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo_);
    ///--- Create color attachments
    _return_image_ = new GLuint[1];
    glGenTextures(1, _return_image_);
    glBindTexture(GL_TEXTURE_2D, _return_image_[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, _imgW_, _imgH_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _return_image_[0], 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    check();
    return _return_image_;
}

void FrameBuffer::check()
{
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "!!!ERROR: Framebuffer is not complete!" << std::endl;
        exit(EXIT_FAILURE);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0); ///< avoid pollution
    check_error_gl();
}

/**
  * @brief cleanup clean all buffers
  */
void FrameBuffer::cleanup(bool CleanAll)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &_fbo_);
    glDeleteRenderbuffers(1, &_depth_buffer_);
    if (CleanAll)
    {
        glDeleteTextures(_layer_number_, _return_image_);
        delete[] _return_image_;
    }
}


#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_
#include "icg_common.h"

/// @remarks Create only one renderbuffer
class FrameBuffer { 
private:
    enum FB_MODE { COLOR, DEPTH } mode;
    /// @name Constants
    int _imgW_;
    int _imgH_;
    GLint   _previewport_[4];
    /// @name fbo contents
    GLuint  _fbo_;  ///< framebuffer object
    GLuint  _depth_buffer_;      ///< depth texture
    GLuint  _layer_number_;      ///< image layer number
    GLuint* _color_attachments_; ///< color buffers
    GLuint* _return_image_;  ///< color attachments
public:
    /**
     * @brief FrameBuffer: Constructor
     * @param image_width
     * @param image_height
     */
    FrameBuffer(int image_width, int image_height):
        _imgW_(image_width),
        _imgH_(image_height){}
    /**
      * @brief ~FrameBuffer: Destructor
      */
    ~FrameBuffer() { cleanup(false); }
    /**
     * @brief bind: bind buffer to draw
     * @param n: starting buffer index
     * @param len: number of buffer to be drawn
     */
    void bind(int offset = 0, int len = 0);
    /**
     * @brief unbind: unbind all textures from current buffer
     */
    void unbind();
    /**
     * @brief init: initialize frame buffer object
     * @param n: number of color texture to be created
     * @param use_interpolation
     * @return fbo parameter
     */
    GLuint* initColor(int n, GLint internalFormat = GL_RGB32F, GLint Format = GL_RGB);
    /**
     * @brief initDepth Initialize only one Depth Buffer
     * @return
     */
    GLuint* initDepth();
    /**
     * @brief check Check FrameBuffer status
     */
    void check();
    /**
     * @brief cleanup clean all buffers
     */
    void cleanup(bool CleanAll = false);
};
#endif

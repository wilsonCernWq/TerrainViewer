//
// Created by qwu on 4/14/17.
//
#pragma once
#ifndef _ICG_OBJECT_NOISE_H_
#define _ICG_OBJECT_NOISE_H_

#include "../common/icg_common.h"
#include "../common/icg_helper.h"
#include "object_datatype.h"
#include "object_abstract.h"

namespace icg {

    class NoiseObject : public AbstractObject {
    private:
        const float quad_vertex[12] = {
                1.0f, 1.0f, 0.0f,
                -1.0f, 1.0f, 0.0f,
                1.0f, -1.0f, 0.0f,
                -1.0f, -1.0f, 0.0f
        };
        cy::GLRenderTexture2D noise;
        unsigned int width, height;
    public:
        virtual cy::GLRenderTexture2D& GetNoiseBuffer() { return noise; }
        virtual bool Init(unsigned int framewidth, unsigned int frameheight)
        {
            width = framewidth;
            height = frameheight;
            return Init();
        }
        virtual bool Init()
        {
            //!
            //!
            AbstractObject::AddNewShaders("vs.noise.glsl", "fs.noise.glsl");
            if (!AbstractObject::CompileShaders()) { return false; }
            //!
            //!
            AbstractObject::BindShader(0);
            VertexArrayObject &vao = AbstractObject::AddNewVAO();
            vao.AddNewVertexBuffer(false, false);
            vao.vbo_data[0].data_ptr = const_cast<float *>(quad_vertex);
            vao.vbo_data[0].data_size = sizeof quad_vertex;
            if (!vao.Init()) { return false; };
            //!
            //! initialize framebuffer object
            if (!noise.Initialize(true, 4, width, height)) {
                std::cerr << "frame buffer incomplete -> "
                          << std::endl;
                return false;
            }
            noise.SetTextureMaxAnisotropy();
            noise.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR);
            noise.SetTextureWrappingMode(GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
            //!
            //!
            check_error_gl("end of noise initialization");
            return true;
        }
        //!
        //!
        virtual void Draw() {
            noise.Bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            AbstractObject::BindBuffer(0, 0);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            noise.Unbind();
            noise.BuildTextureMipmaps();
        }
		virtual void DrawDisplay() {
			AbstractObject::BindBuffer(0, 0);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
    };

};

#endif//_ICG_OBJECT_NOISE_H_

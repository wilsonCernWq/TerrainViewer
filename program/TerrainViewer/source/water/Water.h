#ifndef WATER_H
#define WATER_H
#include "icg_common.h"
#include "../basic/Light.h"
#include "../basic/Object.h"

class Water : public Object {
private:
    GLuint pid;
    GLuint tex_normal; ///< Normal map
private:
    const GLfloat size;
    Light&   sun;
    GLfloat& sandH;
public:
    vec3   water_color = vec3(40.0/255, 76.0/255, 94.0/255);
public:
    Water(Light& light, float& sandheight, const GLfloat scale):
        sun(light), sandH(sandheight), size(scale)
    {}

    void init()
    {
        pid = genProgram("water_vshader.glsl", "water_fshader.glsl");
        bindProgram(0);
        {
            ///--- Vertices
            const GLfloat vpoint[] = { /*V1*/ -size, -size,
                                       /*V2*/ +size, -size,
                                       /*V3*/ -size, +size,
                                       /*V4*/ +size, +size };
            glBindBuffer(GL_ARRAY_BUFFER, genBuffer());
            glBufferData(GL_ARRAY_BUFFER, sizeof(vpoint), vpoint, GL_STATIC_DRAW);
            GLuint id = glGetAttribLocation(pid, "vpoint");
            glEnableVertexAttribArray(id);
            glVertexAttribPointer(id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            ///--- Textures
            glGenTextures(1, &tex_normal);
            glBindTexture(GL_TEXTURE_2D, tex_normal);
            glfwLoadTexture2D("water_texture.tga", 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            SetShaderTexture2D(pid, "waterReflex", "tex_reflex");
            SetShaderTexture2D(pid, "waterNormal", "tex_normal");
        }
        unbindProgram();
    }

    void draw(const mat4& model, const mat4& view, const mat4& projection)
    {
        bindProgram();
        {
            sun.setup(pid);

            ActiveShaderTexture2D("waterNormal", &tex_normal);
            SetShader4m(pid, "MV"  , view * model);
            SetShader4m(pid, "proj", projection  );
            SetShader1f(pid, "time", glfwGetTime());
            SetShader3v(pid, "water_color", water_color);
            SetShader1f(pid, "water_level", sandH-0.1);
            
            ///--- Draw
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        unbindProgram();
    }
};
#endif

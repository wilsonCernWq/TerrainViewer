#ifndef TERRAIN_H
#define TERRAIN_H
#include "terrainface/Terrainface.h"
#include "terrainquad/TerrainCPU.h"
#include "texgridquad/Texgrid.h"
#include "FrameBuffer.h"
#include "Light.h"
#include <cmath>

class Terrain {
public:
    float maxHeight = 0.5;
    float displayRatio = 1.0;
    float terrainRange = 8.0;
private:
    Light& lightSource;
    mat4& ligproj;
    mat4& ligview;
    vec2& origin;
private:
    typedef unsigned char Pixel;
    TexGrid terrainGrid = TexGrid(lightSource, maxHeight, displayRatio, terrainRange);
    TerrainCPU heightMap = TerrainCPU(terrainRange);
    Terrainface terrainFace = Terrainface(maxHeight, displayRatio, terrainRange);
    Pixel* img;
    int imgwidth = 2048;
    int imgheight = 2048;
    mat4 B0, B1, B2, B3, B4;
public:
    Terrain(Light& sun, mat4& lightProj, mat4& lightView, vec2& ori)
        : lightSource(sun), ligproj(lightProj), ligview(lightView), origin(ori){}

    float getWaterHeight() { return terrainGrid.getWaterHeight(); }

    float getTerrainHeight(float x, float y, float sfactor) {
        x = x / sfactor;
        y = y / sfactor;
        if (-1.0 <= x && x < 1.0 && -1.0 <= y && y < 1.0) {
            float u = 0.5 * (1.0 - y) * displayRatio / terrainRange + origin(1);
            float v = 0.5 * (x + 1.0) * displayRatio / terrainRange + origin(0);
            u = fmod(u,2) >= 0 ? fmod(u,2) : 2 + fmod(u,2);
            v = fmod(v,2) >= 0 ? fmod(v,2) : 2 + fmod(v,2);
            if (u >= 1) { u = 2 - u; }
            if (v >= 1) { v = 2 - v; }
            int xcoor = u * imgwidth;
            int ycoor = v * imgheight;
            float h = (float)img[(xcoor * imgwidth + ycoor) * 3] / 255.0;
            float height = h > 0.2 ? h * maxHeight / displayRatio : 0.2 * maxHeight / displayRatio;
            return height * sfactor;
        } else {
            return 0.5 * sfactor;
        }
    }

    void init(GLuint depth_buffer) {
        /// @todo Setup border position
        mat4 R0 = Eigen::Affine3f(Eigen::AngleAxisf( 0.5 * M_PI, Eigen::Vector3f::UnitX())).matrix();
        mat4 R1 = Eigen::Affine3f(Eigen::AngleAxisf(-0.5 * M_PI, Eigen::Vector3f::UnitY())).matrix();
        mat4 P0 = Eigen::Affine3f(Eigen::Translation3f(0.0f, 0.0,  1.0f)).matrix();
        mat4 P1 = Eigen::Affine3f(Eigen::Translation3f(0.0f, 0.0, -1.0f)).matrix();
        mat4 P2 = Eigen::Affine3f(Eigen::Translation3f(0.0f, 0.0,  1.0f)).matrix();
        mat4 P3 = Eigen::Affine3f(Eigen::Translation3f(0.0f, 0.0, -1.0f)).matrix();
        mat4 P4 = Eigen::Affine3f(Eigen::Translation3f(0.0f, 0.0, -1.0f)).matrix();
        B0 = R0 * P0;
        B1 = R0 * P1;
        B2 = R1 * P2;
        B3 = R1 * P3;
        B4 = P4;
        /// @todo Setup height map
        FrameBuffer fb(imgwidth,imgheight);
        GLuint tex = fb.init()[0];
        terrainGrid.setTexture(tex);
        terrainGrid.setDepthBuffer(depth_buffer);
        terrainGrid.init();
        heightMap.init();
        fb.bind(0);
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            heightMap.init(true);
            heightMap.draw();
        }
        fb.unbind();
        fb.cleanup();
        terrainFace.setTexture(tex);
        terrainFace.init();

        img = new Pixel[imgwidth * imgheight * 3];
        glBindTexture(GL_TEXTURE_2D, tex);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)img);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void drawDepth(mat4& M, vec2 origin) {
        terrainGrid.drawDepth(M, ligview, ligproj, origin);
        terrainFace.drawDepth(ligproj * ligview * M * B0, origin, 0);
        terrainFace.drawDepth(ligproj * ligview * M * B1, origin, 1);
        terrainFace.drawDepth(ligproj * ligview * M * B2, origin, 2);
        terrainFace.drawDepth(ligproj * ligview * M * B3, origin, 3);
        terrainFace.drawDepth(ligproj * ligview * M * B4, origin, 4);
    }

    void drawFog(mat4& M, mat4& view, mat4& proj, mat4& trackball, vec2 origin) {
        terrainGrid.drawDepth(trackball * M, view, proj, origin);
        terrainFace.drawDepth(proj * view * trackball * M * B0, origin, 0);
        terrainFace.drawDepth(proj * view * trackball * M * B1, origin, 1);
        terrainFace.drawDepth(proj * view * trackball * M * B2, origin, 2);
        terrainFace.drawDepth(proj * view * trackball * M * B3, origin, 3);
        terrainFace.drawDepth(proj * view * trackball * M * B4, origin, 4);
    }

    void draw(mat4& M, mat4& view, mat4& proj, mat4& trackball, vec2 origin) {
        terrainGrid.draw(trackball, M, view, proj, ligproj * ligview, origin);
        terrainFace.draw(proj * view * trackball * M * B0, origin, 0);
        terrainFace.draw(proj * view * trackball * M * B1, origin, 1);
        terrainFace.draw(proj * view * trackball * M * B2, origin, 2);
        terrainFace.draw(proj * view * trackball * M * B3, origin, 3);
        terrainFace.draw(proj * view * trackball * M * B4, origin, 4);
    }
};
#endif

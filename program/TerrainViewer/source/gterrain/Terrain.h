#ifndef _GTERRAIN_H_
#define _GTERRAIN_H_

#include "../icg_helper.h"
#include "../framebuffer/FrameBuffer.h"
#include "../basic/Light.h"
#include "TerrainMap.h"
#include "TerrainGrid.h"

/**
 * @brief The GTerrain class Render Terrain on GPU
 */
class Terrain {
public:
    ///--- Noise Generation Parameter
    float H = 0.90;
    float L = 4.70;
    float O = 4.00;
    float G = 4.25;
    float B = 0.10;
    ///--- Terrain Geometry
    float terR = 1.0;
    float maxH = 8.0;
    float sandH = 1.0;
    float snowH = 2.4;
private:
    vec2&  _origin_;
    mat4&  _lproj_;
    mat4&  _lview_;
    Light& _light_;
    ///--- Level of Details
    const int maxLodLevel = 7;
    const int chrunkLevel = 3;
public:
    DrawPerlin  perlinMap = DrawPerlin(512 , pow(2.0f,chrunkLevel) * 32.0f, GL_R8, GL_RED);
    TerrainMap  heightMap = TerrainMap(2048, GL_RGBA32F, GL_RGBA, _origin_);
    TerrainGrid terrainGrid = TerrainGrid(_light_, _origin_, maxH, terR, sandH, snowH, maxLodLevel, chrunkLevel);
public:
    Terrain(Light& sun,
            mat4&  lightProj,
            mat4&  lightView,
            vec2&  origin):
        _light_ (sun),
        _lproj_ (lightProj),
        _lview_ (lightView),
        _origin_(origin)
    {}

    GLfloat getMaxScales() { return pow(3.0f, chrunkLevel); }

    void init(GLuint shadow_buffer)
    {
        perlinMap.init();

        heightMap.Parameters(H, L, O, G, B);
        heightMap.setTexture(perlinMap.tex);
        heightMap.init();

        terrainGrid.setDepthTexture(shadow_buffer);
        terrainGrid.setColorTexture(heightMap.tex);
        terrainGrid.setNoiseTexture(perlinMap.tex);
        terrainGrid.init();

        ///--- Draw texture
        perlinMap.drawTexture();
        heightMap.drawTexture();
    }

    void drawDepth(mat4& M)
    {
        terrainGrid.drawDepthLod(M, _lview_, _lproj_);
        //terrainGrid.drawDepth(M, _lview_, _lproj_);
    }

    void draw(mat4& M, mat4& view, mat4& proj, mat4& trackball)
    {
        heightMap.drawTexture();
        terrainGrid.drawLod(trackball, M, view, proj, _lview_, _lproj_);
        //terrainGrid.draw(trackball, M, view, proj, _lview_, _lproj_);
    }
};
#endif // _GTerrain_H_

#ifndef TEXGRID_H
#define TEXGRID_H
#include <vector>
#include <algorithm>
#include "../icg_helper.h"
#include "../basic/Object.h"
#include "../basic/Light.h"

class TerrainGrid: public Object {
private:
    struct VBOLod
    {
        int Offset;
        int Number;
        int Lod;
    };
    struct ChrunkLod
    {
        Transform Matrix;
        vec2  Center;
        float Scale;
        int   Level;
    };
private:
    GLuint resolution;
    ///--- Vertices and Faces
    GLuint VA_buffer;
    GLuint FA_buffer;
    GLuint FA_LOD_buffer;
    ///--- Program id
    GLuint pid_color;
    GLuint pid_color_lod;
    GLuint pid_depth;
    GLuint pid_depth_lod;
    ///--- Textures
    GLuint tex_height;
    GLuint tex_shadow;
    GLuint tex_pnoise;
    GLuint tex_color[6];  ///< color textures
    GLchar* tex_color_filename[6];
    GLchar* tex_color_varyings[6];
    ///--- Other objects
    Light& sun;
    GLfloat& sandH;
    GLfloat& snowH;
    GLfloat& maxH;
    GLfloat& terR;
    vec2& origin_tcoord;
    ///--- Level of Details
    static vec2 mask[8];
    const int& chrunkLevel;
    const int& maxLodLevel;
    std::vector<ChrunkLod> chrunk;
    std::vector<VBOLod>    VBOInfo;
    std::vector<GLfloat> VList;
    std::vector<GLuint > FList;
    std::vector<GLuint > FListLod;
private:
    /**
     * @brief SetupVertices
     */
    void SetupVertices()
    {
        resolution = (int)powf(2.0f, (float)maxLodLevel);
        ///--- To computer vertices position
        for (int i = 0; i < resolution + 1; ++i){
        for (int j = 0; j < resolution + 1; ++j){
            VList.push_back(-1.0f + 2.0f * i / resolution);
            VList.push_back(-1.0f + 2.0f * j / resolution);
            VList.push_back(0.0f);
        }
        }
        ///--- Bind Vertices Buffer
        VA_buffer = genBuffer();
        glBindBuffer(GL_ARRAY_BUFFER, VA_buffer);
        glBufferData(GL_ARRAY_BUFFER, VList.size()*sizeof(GLfloat), &VList[0], GL_STATIC_DRAW);
    }

    /**
     * @brief SetupIndices
     */
    void SetupIndices()
    {
        ///--- To compute face indices
        for (int y = 0; y < resolution; y += 1) {
        for (int x = 0; x < resolution; x += 1) {
            GLuint Ind = y * (resolution + 1) + x;
            // first triangle
            FList.push_back(Ind);
            FList.push_back(Ind + resolution + 1);
            FList.push_back(Ind + 1);
            // second triangle
            FList.push_back(Ind + 1);
            FList.push_back(Ind + resolution + 1);
            FList.push_back(Ind + resolution + 2);
        }
        }
        ///--- Bind Indices Buffer
        FA_buffer = genBuffer();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FA_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, FList.size()*sizeof(GLuint), &FList[0], GL_STATIC_DRAW);
    }

    /**
     * @brief SetupIndicesLod
     */
    void SetupIndicesLod()
    {
        using namespace std;
        ///--- Calculate Chrunk Matrices
        float c = 2.0f;
        float s = 1.0f;
        // 0th center square
        ChrunkLod single_chrunk;
        single_chrunk.Matrix = Eigen::AlignedScaling3f(s,s,s);
        single_chrunk.Center = vec2(0.0f,0.0f);
        single_chrunk.Level = 0;
        single_chrunk.Scale = s;
        chrunk.push_back(single_chrunk);
        // ith chrunk
        for (int i = 0; i < chrunkLevel; ++i)
        {
            for (int j = 0; j < 8; ++j)
            {
                float x = mask[j][0] * c;
                float y = mask[j][1] * c;
                single_chrunk.Matrix  = Eigen::Translation3f(x,y,0.0f);
                single_chrunk.Matrix *= Eigen::AlignedScaling3f(s,s,s);
                single_chrunk.Center  = vec2(x,y);
                single_chrunk.Level = 0;
                single_chrunk.Scale = s;
                chrunk.push_back(single_chrunk);
            }
            s = s * 3.0f;
            c = c * 3.0f;
        }
        ///--- Calculate Lod buffer
        int LastIndex = 0;
        int LodInd = 0;
        int LodStp = 0;
        int LodStr = resolution + 1;
        for (int iLod = 0; iLod <= maxLodLevel; ++iLod)
        {
            LodStp = int(powf(2.0f, float(iLod)));
            for (int y = 0; y < resolution; y += LodStp) {
            for (int x = 0; x < resolution; x += LodStp) {
                LodInd = y * LodStr + x;
                // first triangle
                FListLod.push_back(LodInd);
                FListLod.push_back(LodInd + LodStp * LodStr);
                FListLod.push_back(LodInd + LodStp);
                // second triangle
                FListLod.push_back(LodInd + LodStp);
                FListLod.push_back(LodInd + LodStp * LodStr);
                FListLod.push_back(LodInd + LodStp * LodStr + LodStp);
            }
            }
            // save lod info
            VBOLod info;
            info.Lod    = iLod;
            info.Offset = LastIndex;
            info.Number = FListLod.size() - LastIndex;
            LastIndex   = FListLod.size();
            VBOInfo.push_back(info);
        }
        ///--- Bind buffer to FA_LOD_buffer
        FA_LOD_buffer = genBuffer();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FA_LOD_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, FListLod.size()*sizeof(GLuint), &FListLod[0], GL_STATIC_DRAW);
    }

    /**
     * @brief SetupTexture
     * @param program
     * @param ref
     */
    void SetupTexture(GLuint program, std::string ref = "aa")
    {
        glGenTextures(6, tex_color);
        for (int i = 0; i < 6; ++i)
        {
            glBindTexture(GL_TEXTURE_2D, tex_color[i]);
            glfwLoadTexture2D(tex_color_filename[i], 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
        SetShaderTexture2Ds(program, ref, tex_color_varyings, 6);
        check_error_gl();
    }

public:    
    /**
     * @brief TerrainGrid
     * @param light
     * @param maximumHeight
     * @param terrainRange
     * @param waterHeight
     */
    TerrainGrid(Light& light,
                vec2&  origin,
                GLfloat& maxHeight,
                GLfloat& dispRange,
                GLfloat& sandHeight,
                GLfloat& snowHeight,
                const int& MaxLodLevel,
                const int& ChrunkLevel)
        : sun(light),
          origin_tcoord(origin),
          maxLodLevel(MaxLodLevel),
          chrunkLevel(ChrunkLevel),
          maxH(maxHeight),
          terR(dispRange),
          sandH(sandHeight),
          snowH(snowHeight)
    {
        ///--- Set texture file names
        tex_color_filename[0] = "Highlands.tga";
        tex_color_filename[1] = "Fault_Zone.tga";
        tex_color_filename[2] = "Snowy_Mud.tga";
        tex_color_filename[3] = "Sahara.tga";
        tex_color_filename[4] = "Forested_Mountainside.tga";
        tex_color_filename[5] = "Weedy_Lawn.tga";
        ///--- Set texture varying name
        tex_color_varyings[0] = "color_tex0";
        tex_color_varyings[1] = "color_tex1";
        tex_color_varyings[2] = "color_tex2";
        tex_color_varyings[3] = "color_tex3";
        tex_color_varyings[4] = "color_tex4";
        tex_color_varyings[5] = "color_tex5";
    }

    /**
     * @brief setColorTexture
     * @param tex
     */
    void setColorTexture(GLuint tex) { tex_height = tex; }

    /**
     * @brief setDepthTexture
     * @param tex
     */
    void setDepthTexture(GLuint tex) { tex_shadow = tex; }

    /**
     * @brief setNoiseTexture
     * @param tex
     */
    void setNoiseTexture(GLuint tex) { tex_pnoise = tex; }

    /**
     * @brief init Initialization
     */
    void init()
    {
        /// @todo --- Indices and Vertices
        SetupVertices();
        SetupIndices();
        SetupIndicesLod();

        /// @todo --- Color draw
        pid_color = genProgram("color_vshader.glsl", "color_fshader.glsl");
        bindProgram(0);        
        {
            ///--- Setup Textures
            SetShaderTexture2D(pid_color, "gridColorPNoise", "pnoise_map");
            SetShaderTexture2D(pid_color, "gridColorHeight", "height_map");
            SetShaderTexture2D(pid_color, "gridColorShadow", "shadow_map");
            SetupTexture(pid_color, "gridColorTex");
            ///--- Setup vertices and faces buffer
            glBindBuffer(GL_ARRAY_BUFFER, VA_buffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FA_buffer);
            ///--- Setup Variables
            GLuint color_id = glGetAttribLocation(pid_color, "vpoint");
            glEnableVertexAttribArray(color_id);
            glVertexAttribPointer(color_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }
        unbindProgram();

        /// @todo --- Color Lod draw
        pid_color_lod = genProgram("color_vshader.glsl", "color_fshader.glsl");
        bindProgram(1);
        {
            ///--- Setup Textures
            SetShaderTexture2D(pid_color_lod, "gridColorLodPNoise", "pnoise_map");
            SetShaderTexture2D(pid_color_lod, "gridColorLodHeight", "height_map");
            SetShaderTexture2D(pid_color_lod, "gridColorLodShadow", "shadow_map");
            SetupTexture(pid_color_lod, "gridColorLodTex");
            ///--- Setup vertices and faces buffer
            glBindBuffer(GL_ARRAY_BUFFER, VA_buffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FA_LOD_buffer);
            ///--- Setup Variables
            GLuint color_lod_id = glGetAttribLocation(pid_color_lod, "vpoint");
            glEnableVertexAttribArray(color_lod_id);
            glVertexAttribPointer(color_lod_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }
        unbindProgram();

        /// @todo --- Depth draw
        pid_depth = genProgram("depth_vshader.glsl", "depth_fshader.glsl");
        bindProgram(2);
        {
            ///--- Setup Textures
            SetShaderTexture2D(pid_depth, "gridDepthHeight", "height_map");
            ///--- Setup vertices and faces buffer
            glBindBuffer(GL_ARRAY_BUFFER, VA_buffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FA_buffer);
            ///--- Setup Variables
            GLuint depth_id = glGetAttribLocation(pid_depth, "vpoint");
            glEnableVertexAttribArray(depth_id);
            glVertexAttribPointer(depth_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }
        unbindProgram();

        /// @todo --- Depth Lod draw
        pid_depth_lod = genProgram("depth_vshader.glsl", "depth_fshader.glsl");
        bindProgram(3);
        {
            ///--- Setup Textures
            SetShaderTexture2D(pid_depth_lod, "gridDepthLodHeight", "height_map");
            ///--- Setup vertices and faces buffer
            glBindBuffer(GL_ARRAY_BUFFER, VA_buffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FA_LOD_buffer);
            ///--- Setup Variables
            GLuint depth_lod_id = glGetAttribLocation(pid_depth_lod, "vpoint");
            glEnableVertexAttribArray(depth_lod_id);
            glVertexAttribPointer(depth_lod_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }
        unbindProgram();

        /// @todo --- Check Error
        check_error_gl();
    }

    /**
     * @brief draw
     * @param Trackball
     * @param Model
     * @param View_camera
     * @param Proj_camera
     * @param View_shadow
     * @param Proj_shadow
     * @param origin
     */
    void draw(const mat4& Trackball,
              const mat4& Model,
              const mat4& View_camera,
              const mat4& Proj_camera,
              const mat4& View_shadow,
              const mat4& Proj_shadow)
    {
        ///--- Bind Matrices
        mat4 vMVP = Proj_camera * View_camera * Trackball   * Model;
        mat4 lMVP = sun.offset  * Proj_shadow * View_shadow * Model;
        ///--- Load program
        bindProgram(0);
        {
            sun.setup(pid_color);
            ///--- Bind textures
            ActiveShaderTexture2D("gridColorTex", tex_color);
            ActiveShaderTexture2D("gridColorHeight", &tex_height);
            ActiveShaderTexture2D("gridColorShadow", &tex_shadow);
            ActiveShaderTexture2D("gridColorPNoise", &tex_pnoise);
            ///--- Light and Material
            SetShader1f(pid_color, "sandH"  , sandH);
            SetShader1f(pid_color, "snowH"  , snowH);
            SetShader1f(pid_color, "maxH"  , maxH);
            SetShader1f(pid_color, "terR"  , terR * pow(3.0, chrunkLevel));
            SetShader1f(pid_color, "terS"  , 1.0f);
            SetShader4m(pid_color, "vMVP"  , vMVP);
            SetShader4m(pid_color, "lMVP"  , lMVP);
            SetShader2v(pid_color, "origin_vcoord", vec2(0,0));
            SetShader2v(pid_color, "origin_tcoord", origin_tcoord);
            glDrawElements(GL_TRIANGLES, FList.size()*sizeof(GLuint), GL_UNSIGNED_INT, 0);
        }
        unbindProgram();
        check_error_gl();
    }

    /**
     * @brief drawLod
     * @param Trackball
     * @param Model  model matrix
     * @param View_camera
     * @param Proj_camera
     * @param View_shadow
     * @param Proj_shadow
     * @param origin origin
     */
    void drawLod(const mat4& Trackball,
                 const mat4& Model,
                 const mat4& View_camera,
                 const mat4& Proj_camera,
                 const mat4& View_shadow,
                 const mat4& Proj_shadow)
    {
        ///--- Bind Matrices
        mat4 vMVP = Proj_camera * View_camera * Trackball   * Model;
        mat4 lMVP = sun.offset  * Proj_shadow * View_shadow * Model;
        ///--- Load program
        bindProgram(1);
        {
            sun.setup(pid_color_lod);
            ///--- Bind textures
            ActiveShaderTexture2D("gridColorLodTex", tex_color);
            ActiveShaderTexture2D("gridColorLodHeight", &tex_height);
            ActiveShaderTexture2D("gridColorLodShadow", &tex_shadow);
            ActiveShaderTexture2D("gridColorLodPNoise", &tex_pnoise);
            SetShader1f(pid_color_lod, "sandH"  , sandH);
            SetShader1f(pid_color_lod, "snowH"  , snowH);
            SetShader1f(pid_color_lod, "maxH", maxH);
            SetShader1f(pid_color_lod, "terR", terR * pow(3.0, chrunkLevel));
            SetShader2v(pid_color_lod, "origin_tcoord", origin_tcoord);
            ///--- Calculate Lod level
            int bLod = 0;

            ///--- Do transformation on each chrunk
            bLod = max(min(bLod,maxLodLevel),0);
            for (int i = 0; i < chrunk.size(); ++i)
            {
                int iLod = max(min(bLod + chrunk[i].Level,maxLodLevel),0);
                mat4 vMVP_Lod = vMVP * chrunk[i].Matrix.matrix();
                mat4 lMVP_Lod = lMVP * chrunk[i].Matrix.matrix();
                SetShader1f(pid_color_lod, "terS"  , chrunk[i].Scale);
                SetShader4m(pid_color_lod, "vMVP"  , vMVP_Lod        );
                SetShader4m(pid_color_lod, "lMVP"  , lMVP_Lod        );
                SetShader2v(pid_color_lod, "origin_vcoord", chrunk[i].Center);
                glDrawElements(GL_TRIANGLES, VBOInfo[iLod].Number, GL_UNSIGNED_INT,
                               (GLvoid*)(sizeof(GLuint)*VBOInfo[iLod].Offset));
            }
        }
        unbindProgram();
        check_error_gl();
    }

    /**
     * @brief drawDepth
     * @param Model  model matrix
     * @param View   view matrix
     * @param Proj   projection matrix
     * @param origin origin
     */
    void drawDepth(const mat4& Model, const mat4& View, const mat4& Proj)
    {
        mat4 MVP =  Proj * View * Model;
        check_error_gl();
        bindProgram(2);
        {
            ActiveShaderTexture2D("gridDepthHeight", &tex_height);
            SetShader4m(pid_depth, "MVP"   , MVP );
            SetShader1f(pid_depth, "maxH"  , maxH);
            SetShader1f(pid_depth, "terR"  , terR * pow(3.0, chrunkLevel));
            SetShader1f(pid_depth, "terS"  , 1.0f);
            SetShader2v(pid_depth, "origin_vcoord", vec2(0,0));
            SetShader2v(pid_depth, "origin_tcoord", origin_tcoord);
            glDrawElements(GL_TRIANGLES, FList.size()*sizeof(GLuint), GL_UNSIGNED_INT, 0);
        }
        unbindProgram();
        check_error_gl();
    }

    /**
     * @brief drawDepthLod
     * @param Model  model matrix
     * @param View   view matrix
     * @param Proj   projection matrix
     * @param origin origin
     */
    void drawDepthLod(const mat4& Model, const mat4& View, const mat4& Proj)
    {
        mat4 MVP =  Proj * View * Model;
        bindProgram(3);
        {
            ActiveShaderTexture2D("gridDepthLodHeight", &tex_height);
            SetShader1f(pid_depth_lod, "maxH", maxH);
            SetShader1f(pid_depth_lod, "terR", terR * pow(3.0, chrunkLevel));
            SetShader2v(pid_depth_lod, "origin_tcoord", origin_tcoord);

            ///--- Calculate Lod level
            int bLod = 0;

            ///--- Do transformation on each chrunk
            bLod = max(min(bLod,maxLodLevel),0);
            for (int i = 0; i < chrunk.size(); ++i)
            {
                int iLod = max(min(bLod + chrunk[i].Level,maxLodLevel),0);
                mat4 MVP_Lod = MVP * chrunk[i].Matrix.matrix();
                SetShader4m(pid_depth_lod, "MVP"   , MVP_Lod        );
                SetShader1f(pid_depth_lod, "terS"  , chrunk[i].Scale);
                SetShader2v(pid_depth_lod, "origin_vcoord", chrunk[i].Center);
                glDrawElements(GL_TRIANGLES, VBOInfo[iLod].Number, GL_UNSIGNED_INT,
                               (GLvoid*)(sizeof(GLuint)*VBOInfo[iLod].Offset));
            }
        }
        unbindProgram();
        check_error_gl();
    }
};

/** Initialize Mask */
vec2 TerrainGrid::mask[8] = {
    vec2(-1, 1),vec2(0, 1),vec2(1, 1),
    vec2(-1, 0),           vec2(1, 0),
    vec2(-1,-1),vec2(0,-1),vec2(1,-1)
};

#endif

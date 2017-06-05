#pragma once
#ifndef _OBJECT_TERRAIN_MAP_H_
#define _OBJECT_TERRAIN_MAP_H_

#include "../common/icg_common.h"
#include "../common/icg_helper.h"
#include "../camera.h"
#include "object_datatype.h"
#include "object_abstract.h"
#include "object_light.h"
#include "object_noise.h"

namespace icg {

    class TerrainObject : public AbstractObject, public GeometryObject {
	public:
		float octave = 4.0f;
		float persistence = 0.9; 
		float lacunarity = 4.7; 
		float gain = 4.3;       
		float offset = 0.1; 
		float movespeed = 0.01f;
		cy::Point2f origin = cy::Point2f(0.2f, 0.2f);
		float sandlevel = 0.05f;
		float snowlevel = 0.15f;
		float maxlevel = 0.3f;
		void MoveUp() { origin.x += movespeed; }
		void MoveDown() { origin.x -= movespeed; }
		void MoveLeft() { origin.y += movespeed; }
		void MoveRight() { origin.y -= movespeed; }
    private:
		struct GLMapInfo 
		{
			icg::ImageData map;
			cy::GLTexture2D tex;
			bool isempty = true;
			GLMapInfo(std::string filename) {
				helper::loadimg(this->map, filename);
				if (this->map.data.size() > 0) {
					this->tex.Initialize();
					this->tex.SetImage(map.internalFormat, map.format, map.type, map.data.data(), map.width, map.height);
					this->tex.BuildMipmaps();
					this->tex.SetMaxAnisotropy();
					this->tex.SetFilteringMode(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
				}
				else {
					std::cerr << "Error: empty texture file " << filename << std::endl;
				}
			}
		};
		std::vector<GLMapInfo> maps;
        //!
        //! The mesh defines the terrain baseline
        std::vector<float> vtptr;
        //!
        //!
        LightObject* light = nullptr;
        NoiseObject* noise = nullptr;
		bool drawGeometryOnly = false;
    public:
        virtual void RegisterLight(LightObject& l) { light = &l; }
        virtual void RegisterNoise(NoiseObject& n) { noise = &n; }
        // How quad indices are assigned
        //        e2
        //    2.______.3
        //     |      |
        //  e3 |      | e1
        //     |______|
        //    0        1
        //        e0
        //
        // Case:
        //  c0 : e0 left  -> v0 = 2, v1 = 1
        //  c1 : e0 right -> v0 = 1, v1 = 2
        //
        //  c2 : e1 upper -> v1 = 1, v3 = 2
        //  c3 : e1 lower -> v1 = 2, v3 = 1
        //
        //  c4 : e2 left  -> v2 = 2, v3 = 1
        //  c5 : e2 right -> v2 = 1, v3 = 2
        //
        //  c6 : e3 upper -> v2 = 2, v0 = 1
        //  c7 : e3 lower -> v2 = 1, v0 = 2
        //
        //  e8  : e0 + e1 -> v1 = 1, v0 = v3 = 2
        //  e9  : e1 + e2 -> v3 = 1, v1 = v2 = 2
        //  e10 : e2 + e3 -> v2 = 1, v0 = v3 = 2
        //  e11 : e3 + e0 -> v0 = 1, v1 = v2 = 2
        //
        // LOD center
        //  edge = 1 + 1 or 1 + 0 or 0 + 1 or 0 + 0 -> use center
        //  edge = 1 + 2 or 2 + 1                   -> use point with mark 2
        //
        void CreateQuad(int i, int j, float step, int c = -1 /* case */, float scale = 1.0f)
        {
            // vertex 0
            vtptr.push_back(i * step);
            vtptr.push_back(j * step);
            vtptr.push_back(c == 0 || c == 7 || c == 8 || c == 10 ?
                            2.0f : (c == 1 || c == 6 || c == 11 ?
                                    1.0f : 0.0f));
			vtptr.push_back(scale);
            // vertex 1
            vtptr.push_back((i + 1) * step);
            vtptr.push_back(j       * step);
            vtptr.push_back(c == 1 || c == 3 || c == 9 || c == 11 ?
                            2.0f : (c == 0 || c == 2 || c == 8 ?
                                    1.0f : 0.0f));
			vtptr.push_back(scale);
            // vertex 2
            vtptr.push_back(i       * step);
            vtptr.push_back((j + 1) * step);
            vtptr.push_back(c == 4 || c == 6 || c == 9 || c == 11 ?
                            2.0f : (c == 5 || c == 7 || c == 10 ?
                                    1.0f : 0.0f));
			vtptr.push_back(scale);
            // vertex 3
            vtptr.push_back((i + 1) * step);
            vtptr.push_back((j + 1) * step);
            vtptr.push_back(c == 2 || c == 5 || c == 8 || c == 10 ?
                            2.0f : (c == 3 || c == 4 || c == 9 ?
                                    1.0f : 0.0f));
			vtptr.push_back(scale);
        }
        void GenerateVertices() {
            const int max_level = 8;
            for (int level = -1; level < max_level; ++level) {
                float step = 2.0f / powf(2.0f, max_level - std::max(level, 0) + 1);
				float scale = 2.0f / step;
				if (level == -1) {
                    CreateQuad(-1, 0, step, -1, scale);
                    CreateQuad(0, 0, step, -1, scale);
                    CreateQuad(0, -1, step, -1, scale);
                    CreateQuad(-1, -1, step, -1, scale);
                }
                else {
                    CreateQuad(-2, 1, step, 10, scale);
                    CreateQuad(-1, 1, step, 4, scale);
                    CreateQuad(0, 1, step, 5, scale);
                    CreateQuad(1, 1, step, 9, scale);
                    CreateQuad(1, 0, step, 2, scale);
                    CreateQuad(1, -1, step, 3, scale);
                    CreateQuad(1, -2, step, 8, scale);
                    CreateQuad(0, -2, step, 1, scale);
                    CreateQuad(-1, -2, step, 0, scale);
                    CreateQuad(-2, -2, step, 11, scale);
                    CreateQuad(-2, -1, step, 7, scale);
                    CreateQuad(-2, 0, step, 6, scale);
                }
            }
        }
        virtual bool Init()
        {
            //!
            //! initialize mesh grid
            GenerateVertices();
            //!
            //! initialize matrix => to centralize the mesh object
            GeometryObject::uscale = 20.0f;
            GeometryObject::center = cy::Point3f(0, 0, 0);
            GeometryObject::scale.SetScaledIdentity(GeometryObject::uscale);
            GeometryObject::model *= cy::Matrix4f::MatrixRotationX(-cy::cyPi<float>() / 2);
            GeometryObject::model *= cy::Matrix4f::MatrixRotationZ(-cy::cyPi<float>() / 2);
            GeometryObject::UpdateMatrix();
            //!
            //! initialize program and shaders
            AbstractObject::AddNewShaders(
                    "vs.terrainobject.glsl",
                    "fs.terrainobject.glsl",
                    "gs.terrainobject.glsl",
                    "tc.terrainobject.glsl",
                    "te.terrainobject.glsl");
            if (!AbstractObject::CompileShaders()) { return false; };
			//!
			//!
			maps.emplace_back("Highlands.tga");
			maps.emplace_back("Fault_Zone.tga");
			maps.emplace_back("Snowy_Mud.tga");
			maps.emplace_back("Sahara.tga");
			maps.emplace_back("Forested_Mountainside.tga");
			maps.emplace_back("Weedy_Lawn.tga");
            //!
            //! initialize VAO data
            VertexArrayObject& vao = AbstractObject::AddNewVAO();
            vao.vbo_data.emplace_back(); // vertex data
            vao.vbo_data[0].data_ptr = vtptr.data();
            vao.vbo_data[0].data_size = sizeof(float) * vtptr.size();
			vao.vbo_data[0].data_width = 4;
            if (!vao.Init()) { return false; };
            check_error_gl("end of quad total initialization");
            return true;
        }
        //!
        //!
        virtual void UpdateUniforms()
        {
			// flags
            shaders[0].shader.SetUniform("sysmode", (int)GLDEBUG); 
			shaders[0].shader.SetUniform("geomode", (int)drawGeometryOnly);
            // matrices
			shaders[0].shader.SetUniform("v", (*GeometryObject::camera)->GetView());
			shaders[0].shader.SetUniform("sm", GeometryObject::sm);
            shaders[0].shader.SetUniform("smv",  GeometryObject::smv);
			shaders[0].shader.SetUniform("smvp", GeometryObject::smvp);
			shaders[0].shader.SetUniform("smv_tinv", GeometryObject::smv_tinv);
			shaders[0].shader.SetUniform("shadowmvp", light->GetShadowMatrix());
			// textures
			shaders[0].shader.SetUniform("noise", 0); noise->GetNoiseBuffer().BindTexture(0);
			shaders[0].shader.SetUniform("shadowtex", 1); light->GetShadowMap().BindTexture(1);
			for (int offset = 2, i = offset; i < offset + maps.size(); ++i) {
				std::string texname = (std::string("featuretex") + std::to_string(i - offset));
				shaders[0].shader.SetUniform(texname.c_str() , i);
				maps[i - offset].tex.Bind(i);
			}
			// parameters
			shaders[0].shader.SetUniform("O", octave);
			shaders[0].shader.SetUniform("P", persistence);
			shaders[0].shader.SetUniform("L", lacunarity);
			shaders[0].shader.SetUniform("G", gain);
			shaders[0].shader.SetUniform("B", offset);
			shaders[0].shader.SetUniform("origin", origin);
			shaders[0].shader.SetUniform("sandH", sandlevel);
			shaders[0].shader.SetUniform("snowH", snowlevel);
			shaders[0].shader.SetUniform("maxH", maxlevel);
			// other
            shaders[0].shader.SetUniform("lightDirCameraCoord", light->GetCameraSpaceLightDirection()); // screen space
            shaders[0].shader.SetUniform("lodCenter", (*GeometryObject::camera)->GetCurrentPosition());
			shaders[0].shader.SetUniform("znear", (*GeometryObject::camera)->GetNearClip());
			shaders[0].shader.SetUniform("zfar", (*GeometryObject::camera)->GetFarClip());
        }
        //!
        //!
		virtual void Draw(bool geomode) {
			drawGeometryOnly = geomode;
			this->Draw();
		}
        virtual void Draw() {
            BindBuffer(0, 0);
            UpdateUniforms();
            glPatchParameteri(GL_PATCH_VERTICES, 4);
            glDrawArrays(GL_PATCHES, 0, vtptr.size());
        }

    };

};
#endif//_OBJECT_TERRAIN_MAP_H_

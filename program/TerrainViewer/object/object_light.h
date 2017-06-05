#pragma once
#ifndef _OBJECT_LIGHT_H_
#define _OBJECT_LIGHT_H_

#include "../common/icg_common.h"
#include "../common/icg_helper.h"
#include "../camera.h"
#include "object_datatype.h"
#include "object_abstract.h"

namespace icg {

	class LightObject : public AbstractObject, public GeometryObject, public MeshObject {
	private:
		const float bias_matrix[16] = {
			0.5f, 0.0f, 0.000f, 0.0f,
			0.0f, 0.5f, 0.000f, 0.0f,
			0.0f, 0.0f, 0.500f, 0.0f,
			0.5f, 0.5f, 0.500f, 1.0f
		};
		const float quad_vertex[12] = {
			1.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f
		};
		//!
		//!
		Camera* oldcam = nullptr;
		Camera lightcam = Camera(Camera::CameraMode::ORGHOGONAL); //! light is a camera also
        cy::Point3f lightcameracoord;
		//!
		//!
		cy::GLRenderDepth2D shadow_map;
		cy::Matrix4f        shadow_smvp;
		GLuint shadow_width = 4096, shadow_height = 4096;
	public:
		//!
		//!
		LightObject() : AbstractObject() {
			this->lightcam.SetInitPosition(cy::Point3f(-9.5f, 3.2f, 0.3f));
			this->lightcam.SetInitFocus(cy::Point3f(0.0f, 0.0f, 0.0f));
			this->lightcam.SetInitUp(cy::Point3f(0.004f, 0.113f, -0.993f));
			this->GeometryObject::RegisterBall(lightcam.GetBall());
		}
		virtual ~LightObject() {}
		//!
		//!
        virtual Camera& GetCamera() { return lightcam; }
        virtual cy::Point3f& GetWorldSpaceLightPosition() {
			helper::debug(lightcam.GetCurrentPosition());
            return lightcam.GetCurrentPosition();
        }
        virtual cy::Point3f& GetCameraSpaceLightDirection() {
            lightcameracoord = cy::Point3f(GeometryObject::GetView() * cy::Point4f(lightcam.GetCurrentPosition(), 0.0f));
            return lightcameracoord;
        }
		virtual cy::GLRenderDepth2D& GetShadowMap() { return shadow_map; }
		virtual cy::Matrix4f& GetShadowMatrix() { return shadow_smvp; }
		//!
		//! Initialization
		virtual bool Init()
		{
            //!
			//! initialize program and shaders
			AbstractObject::AddNewShaders("vs.light.glsl", "fs.light.glsl"); 
			AbstractObject::AddNewShaders("vs.light.debug.glsl", "fs.light.debug.glsl"); 
			if (!AbstractObject::CompileShaders()) { return false; }; 
            //!
			//! init obj mesh
			{
				AbstractObject::BindShader(0);
				VertexArrayObject& vao = AbstractObject::AddNewVAO();
				MeshObject::RegisterMesh("sphere.obj");
				if (!MeshObject::Load()) { return false; };
				if (!MeshObject::InitData(vao, true, false)) { return false; };
				if (!vao.Init()) { return false; }
			}
			//!
			//! initialize VAO data debug quad
			{
				AbstractObject::BindShader(1);
				VertexArrayObject& vao = AbstractObject::AddNewVAO();
				vao.AddNewVertexBuffer(false, false);
				vao.vbo_data[0].data_ptr = const_cast<float*>(quad_vertex);
				vao.vbo_data[0].data_size = sizeof quad_vertex;
				if (!vao.Init()) { return false; };
			}
			//!
			//! initialize mesh geometry
			GeometryObject::uscale = 0.01f;
			GeometryObject::center = (MeshObject::loader.GetBoundMax() + MeshObject::loader.GetBoundMin()) * GeometryObject::uscale / 2;
			GeometryObject::scale.SetScaledIdentity(GeometryObject::uscale);
			GeometryObject::scale.AddTrans(-GeometryObject::center);
			this->UpdateMatrix();
			//!
			//! initialize FBO
			if (!shadow_map.Initialize(false, shadow_width, shadow_height)) {
				std::cerr << "frame buffer incomplete -> " 
					<< "width " << shadow_width 
					<< "height " << shadow_height << std::endl;
				return false;
			}
			shadow_map.SetTextureMaxAnisotropy();
			shadow_map.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR);
			check_error_gl("end of light initialization");
			return true;
		}
		//!
		//! Bind objects
		virtual void BindShadow()
		{
			oldcam = *GeometryObject::camera;
			*GeometryObject::camera = &lightcam;
			shadow_map.Bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		virtual void UnbindShadow()
		{
			shadow_map.Unbind();
			shadow_map.BuildTextureMipmaps();
			shadow_smvp = cy::Matrix4f(bias_matrix) * lightcam.GetProjection() * lightcam.GetView();
			*GeometryObject::camera = oldcam;
			oldcam = nullptr;
		}
		//!
		//!
		virtual void UpdateUniforms(unsigned int shader_id)
		{
			switch (shader_id) {
			case(0):
				shaders[0].shader.SetUniform("mvp", GeometryObject::smvp);
				break;
			case(1):
				shaders[1].shader.SetUniform("tex", 0);
				shadow_map.BindTexture(0);
				break;
			}
		}
		virtual void UpdateMatrix()
		{
			GeometryObject::model.SetRotationX(-cy::cyPi<float>() / 2.0f);
			GeometryObject::model.AddTrans(this->lightcam.GetCurrentPosition());
			this->sm = this->model * this->scale;
			this->smv = (*camera)->GetView() * sm;
			this->smvp = (*camera)->GetProjection() * smv;
			this->smv_tinv = smv.GetTranspose().GetInverse();
		}
		//!
		//!
		virtual void Draw() {
			AbstractObject::BindBuffer(0, 0);
			UpdateUniforms(0);
			glDrawArrays(GL_TRIANGLES, 0, AbstractObject::vaos[0].vertex_num);
		}
		virtual void DrawDebugQuad() {
			AbstractObject::BindBuffer(1, 1);
			UpdateUniforms(1);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}

	};

};

#endif//_OBJECT_LIGHT_H_

#pragma once
#ifndef _OBJECT_DATATYPE_H_
#define _OBJECT_DATATYPE_H_

#include "../common/icg_common.h"
#include "../common/icg_helper.h"
#include "../loader/loader.h"
#include "../camera.h"

namespace icg {
	//! components
	struct VertexBufferData {
		GLvoid *data_ptr = nullptr;
		GLuint  data_size = 0, data_width = 3;
	};

	struct VertexArrayObject {
		GLuint vao = 0, veo = 0, *vbo = nullptr;
		std::vector<VertexBufferData> vbo_data;
		GLvoid *index_ptr = nullptr;
		GLuint index_size = 0, vertex_num = 0;
		//!
		//!
		~VertexArrayObject() {}
		//!
		//!
		void Clean() {
			if (vao != 0) { glDeleteVertexArrays(1, &vao); }
			if (vbo != nullptr) {
				glDeleteBuffers(vbo_data.size(), vbo);
				delete[] vbo;
			}
			if (veo != 0) { glDeleteBuffers(1, &veo); }
		}
		void AddNewVertexBuffer(bool has_normal, bool has_vertex) {
			vbo_data.emplace_back();
			if (has_normal) { vbo_data.emplace_back(); }
			if (has_vertex) { vbo_data.emplace_back(); }
		}
		//!
		//!
		bool Init(bool has_element_buffer = false) {
			// Vertex Array Object
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			// Vertex Buffer Objects
			if (vbo != nullptr) { delete[] vbo; }
			vbo = new GLuint[vbo_data.size()];
			glGenBuffers(vbo_data.size(), vbo);
			for (int i = 0; i < vbo_data.size(); ++i) {
				glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
				glBufferData(GL_ARRAY_BUFFER,
							 vbo_data[i].data_size,
							 vbo_data[i].data_ptr,
							 GL_STATIC_DRAW);
			}
			// Vertex Element Buffer Objects
			if (has_element_buffer) {
				glGenBuffers(1, &veo);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veo);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, index_ptr, GL_STATIC_DRAW);
			}
			check_error_gl("Initialize Vertex Array Object");
			return true;
		}
		//!
		//!
		void Enable(bool has_element_buffer = false) {
			glBindVertexArray(vao);
			if (has_element_buffer) { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veo); }
			for (int i = 0; i < vbo_data.size(); ++i) {
				glEnableVertexAttribArray(i);
				glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
				glVertexAttribPointer(i, vbo_data[i].data_width, GL_FLOAT, GL_FALSE, 0, 0);
			}
			check_error_gl("Enable Vertex Array Object");
		}

	};
};

namespace icg {

	class MeshObject {
	protected:
		const char *filename = nullptr;
		icg::Loader loader;
	public:
		//!
		//!
		MeshObject() : loader(true) {}
		virtual ~MeshObject() {}
		//!
		//!
		virtual void RegisterMesh(const char* filename)
		{
			this->filename = filename;
		}
		//!
		//!
		virtual bool InitData(VertexArrayObject& vao, bool use_normal, bool use_texture) {
			vao.AddNewVertexBuffer(loader.HasNormals() && use_normal,
								   loader.HasTextureVertices() && use_texture);
			loader.ComputeArrayData();
			loader.GetBufferVertex(vao.vbo_data[0].data_ptr, vao.vbo_data[0].data_size, false);
			if (loader.HasNormals() && use_normal) {
				loader.GetBufferNormal(vao.vbo_data[1].data_ptr, vao.vbo_data[1].data_size, false);
			}
			if (loader.HasTextureVertices() && use_texture) {
				loader.GetBufferNormal(vao.vbo_data[1].data_ptr, vao.vbo_data[1].data_size, false);
			}
			loader.GetBufferIndex(vao.index_ptr, vao.index_size, vao.vertex_num, false);
			return true;
		}
		virtual bool Load()
		{
			if (filename != nullptr)
			{
				cy::Timer time;
#ifndef NDEBUG
				std::cout << "Loading mesh from file " << filename << std::endl;
#endif
				time.Start();
				if (!loader.LoadFromFileObj(filename, true))
				{
					std::cerr << "Failed to load mesh after " << time.Stop()
							  << " seconds" << std::endl;
					loader.PrintError();
					return false;
				}
				if (!loader.IsBoundBoxReady()) { loader.ComputeBoundingBox(); }
				if (!loader.HasNormals()) { loader.ComputeNormals(); }
				std::cout << "Loading takes " << time.Stop() << " seconds" << std::endl;
				std::cout
						<< "There are ("
						<< loader.NV()
						<< ") vertices "
						<< " & ("
						<< loader.NVN()
						<< ") vertex normals found in the mesh"
						<< std::endl << std::endl;
			}
			return true;
		}

	};

	class GeometryObject {
	protected:
		//!
		//!
		GLfloat uscale = -1.0f; // uniform scaling
		cy::Point3f	center = cy::Point3f(0, 0, 0);
		//!
		//! matrix transformations
		cy::Matrix4f scale;
		cy::Matrix4f model;
		icg::Camera**camera = nullptr;
		icg::Ball   *ball = nullptr;
		//!
		//! precomputed variables
		cy::Matrix4f sm;
		cy::Matrix4f smv;
		cy::Matrix4f smvp;
		cy::Matrix4f smv_tinv;
	public:
		//!
		//!
		GeometryObject() {
			model.SetIdentity();
			scale.SetIdentity();
		}
		virtual ~GeometryObject() {}
		//!
		//!
		virtual void RegisterCamera(icg::Camera*& camera) { this->camera = &camera; }
		virtual void RegisterBall(icg::Ball* ball) { this->ball = ball; }
		//!
		//!
		icg::Ball* GetBall() { return this->ball; }
		virtual GLfloat& GetScale() { return uscale; }
		virtual cy::Point3f& GetCenter() { return center; }
		virtual cy::Matrix4f& GetView() { return (*camera)->GetView(); }
		virtual cy::Matrix4f& GetProjection() { return (*camera)->GetProjection(); }
		//!
		//!
		virtual void UpdateMatrix()
		{
			assert(*camera != nullptr);
			this->sm = this->model * this->scale;
			this->smv = ball != nullptr ? (*camera)->GetView() * ball->Matrix() * sm : (*camera)->GetView() * sm;
			this->smvp = (*camera)->GetProjection() * smv;
			this->smv_tinv = smv.GetTranspose().GetInverse();
		}
	};

	class ShadowObject {
	protected:
		std::vector<cy::GLRenderDepth2D*> shadow_map;
		std::vector<cy::Matrix4f*> shadow_matrix;
	public:
		//!
		//!
		virtual ~ShadowObject() {}
		//!
		//!
		virtual void AddNewShadow(cy::GLRenderDepth2D& shadow_map, cy::Matrix4f& shadow_matrix)
		{
			this->shadow_map.push_back(&shadow_map);
			this->shadow_matrix.push_back(&shadow_matrix);
		}
	};



};

#endif//_OBJECT_DATATYPE_H_

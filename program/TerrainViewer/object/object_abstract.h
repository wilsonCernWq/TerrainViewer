#pragma once
#ifndef _OBJECT_ABSTRACT_H_
#define _OBJECT_ABSTRACT_H_

#include "../common/icg_common.h"
#include "../common/icg_helper.h"
#include "../ball/ball.h"
#include "../loader/loader.h"
#include "object_datatype.h"

namespace icg {

	class AbstractObject {
	public:
		struct GLProgram {
			cy::GLSLProgram shader;
			std::string vshader, fshader, gshader, tcshader, teshader;
		};
	protected:
		//!
		//!
		int current_shader = -1;
		std::vector<GLProgram> shaders;
		//!
		//! 
		int current_vao = -1;
		std::vector<VertexArrayObject> vaos;
	protected:
		//!
		//! Register external fields
		virtual GLProgram& AddNewShaders(
			const char *vertexShaderFile,
			const char *fragmentShaderFile,
			const char *geometryShaderFile = nullptr,
			const char *tessControlShaderFile = nullptr,
			const char *tessEvaluationShaderFile = nullptr
		)
		{
			shaders.emplace_back();
			shaders.back().vshader = vertexShaderFile;
			shaders.back().fshader = fragmentShaderFile;
			if (geometryShaderFile) { shaders.back().gshader = geometryShaderFile; }
			if (tessControlShaderFile) { shaders.back().tcshader = tessControlShaderFile; }
			if (tessEvaluationShaderFile) { shaders.back().teshader = tessEvaluationShaderFile; }
			return shaders.back();
		}
		virtual VertexArrayObject& AddNewVAO() {
			vaos.emplace_back();
			return vaos.back();
		}
		//!
		//! Bind functions
		virtual void BindShader(int shader_id)
		{
			current_shader = shader_id;
			shaders[current_shader].shader.Bind();
		}
		virtual void BindBuffer(unsigned int shader_id, unsigned int vao_id)
		{
			int pid; glGetIntegerv(GL_CURRENT_PROGRAM, &pid);
			if (pid != shaders[shader_id].shader.GetID() || current_vao != vao_id) {
				AbstractObject::BindShader(shader_id);
				vaos[vao_id].Enable(false);
				current_vao = vao_id;
			}
		}
	public:
		//!
		//! Constructors
		AbstractObject() {}
		virtual ~AbstractObject() {}
		//!
		//!
		virtual bool CompileShaders()
		{
			// compile shaders
			for (auto i = 0; i < shaders.size(); ++i) {
				if (!shaders[i].shader.BuildFiles(
					shaders[i].vshader.c_str(), 
					shaders[i].fshader.c_str(),
					shaders[i].gshader == "" ? nullptr : shaders[i].gshader.c_str(),
					shaders[i].tcshader == "" ? nullptr : shaders[i].tcshader.c_str(),
					shaders[i].teshader == "" ? nullptr : shaders[i].teshader.c_str())) {
					std::cerr << "Failed to compile shaders" << std::endl;
					return false;
				}
			}
			// check error
			check_error_gl("Abstract object shader initialization");
			return true;
		}
		//!
		//!
		virtual bool Init() = 0;
		//!
		//! Draw
		virtual void Draw() = 0;
	};

};

#endif//_OBJECT_ABSTRACT_H_

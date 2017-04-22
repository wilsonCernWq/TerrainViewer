#pragma once
#ifndef _ICG_MESH_WRAPPER_H_
#define _ICG_MESH_WRAPPER_H_

//
// trying this obj loader https://github.com/syoyo/tinyobjloader
//
#include "tiny_obj_loader.h"
#include "../common/icg_common.h"
#include "../common/icg_helper.h"

#ifdef TINY_OBJ_LOADER_H_
const static bool hasTinyLoader = true;
#else
const static bool hasTinyLoader = false;
#endif

namespace icg {

	class Loader : public cy::TriMesh {
	private:
		// reference http://stackoverflow.com/questions/3071665/getting-a-directory-name-from-a-filename
		std::string SplitPath(const std::string& str)
		{
			size_t i = str.find_last_of("/\\");
			if (i != std::string::npos) {
				return str.substr(0, i + 1);
			}
			else {
				return std::string("");
			}
		}
		struct {
			std::vector<cy::Point3f> vtptr;
			std::vector<cy::Point3f> nmptr;
			std::vector<cy::Point3f> txptr;
			unsigned int face_num;
		} __arraydata__;
		struct {
			std::vector<cy::Point3f> nmptr;
			std::vector<cy::Point3f> txptr;
			std::vector<int> nmcount, txcount;
		} __indexdata__;
		struct {
			tinyobj::attrib_t attributes;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;
		} __tiny__;
		std::vector<std::vector<unsigned int>>__mtlData__;
		const bool __useTinyLoader__ = true; // flag to use tiny obj loader or not
		std::string __dirpath__; // directory path to the mesh folder
		std::string __err__ = "";
	public:

		/** Constructor */
		Loader(bool tinyloader = false) : __useTinyLoader__(tinyloader), TriMesh() {}

		/** Get Mesh Path */
		std::string DirPath() { return __dirpath__; }

		/** Get Error Message */
		std::string Error() { return __err__; }
		void PrintError() { if (!__err__.empty()) { std::cerr << __err__ << std::endl; } }

		/** 
		 * Overriding LoadFromFileObj function for TriMesh, force to triangulate
		 */
		bool LoadFromFileObj(const char* fname, bool loadMtl = true) {
			if (__useTinyLoader__ && hasTinyLoader) {
				std::cout << "Using TinyObjLoader" << std::endl;
				__dirpath__ = SplitPath(fname);
				__err__ = "";
				bool succeed = tinyobj::LoadObj(
					&(__tiny__.attributes),
					&(__tiny__.shapes),
					&(__tiny__.materials),
					&__err__,
					fname, 
					__dirpath__ == "" ? nullptr : __dirpath__.c_str(),
					true);
#ifndef NDEBUG
				if (!__err__.empty()) { std::cerr << __err__ << std::endl; }
#endif
				if (!succeed) { return false; }
				else {
					//
					// retrieve information from TinyObjLoader to TriMesh
					std::vector<cy::Point3f> _vx_;
					std::vector<cy::TriMesh::TriFace> _fx_;
					std::vector<cy::Point3f> _vn_;
					std::vector<cy::TriMesh::TriFace> _fn_;
					std::vector<cy::Point3f> _vt_;
					std::vector<cy::TriMesh::TriFace> _ft_;
					__mtlData__ = std::vector<std::vector<unsigned int>>(__tiny__.materials.size() + 1, std::vector<unsigned int>());
					//
					// -- reference https://github.com/syoyo/tinyobjloader
					TriMesh::v = new cy::Point3f[__tiny__.attributes.vertices.size() / 3];
					TriMesh::vn = new cy::Point3f[__tiny__.attributes.normals.size() / 3];
					TriMesh::vt = new cy::Point3f[__tiny__.attributes.texcoords.size() / 2];
					if (__tiny__.attributes.vertices.size() <= 0) { return false; }
					bool has_normals = (__tiny__.attributes.normals.size() > 0);
					bool has_texcoords = (__tiny__.attributes.texcoords.size() > 0);
					for (size_t s = 0; s < __tiny__.shapes.size(); s++) { // Loop over shapes (each shape means a continious mesh)
						size_t index_offset = 0;
						for (size_t f = 0; f < __tiny__.shapes[s].mesh.num_face_vertices.size(); f++) { // Loop over faces (polygon)
							int fv = __tiny__.shapes[s].mesh.num_face_vertices[f]; // number of vertices of this face
							if (fv != 3) { break; return false; }
							cy::TriMesh::TriFace _fx_idx_, _fn_idx_, _ft_idx_;
							for (size_t v = 0; v < fv; v++) { // Loop over vertices in the face.
								tinyobj::index_t idx = __tiny__.shapes[s].mesh.indices[index_offset + v];
								float vx = __tiny__.attributes.vertices[3 * idx.vertex_index + 0];
								float vy = __tiny__.attributes.vertices[3 * idx.vertex_index + 1];
								float vz = __tiny__.attributes.vertices[3 * idx.vertex_index + 2];
								_vx_.emplace_back(vx, vy, vz);
								_fx_idx_.v[v] = _vx_.size() - 1;
								if (has_normals) {
									float nx = __tiny__.attributes.normals[3 * idx.normal_index + 0];
									float ny = __tiny__.attributes.normals[3 * idx.normal_index + 1];
									float nz = __tiny__.attributes.normals[3 * idx.normal_index + 2];
									_vn_.emplace_back(nx, ny, nz);
									_fn_idx_.v[v] = _vn_.size() - 1;
								}
								if (has_texcoords) {
									if (idx.texcoord_index >= 0) {
										float tx = __tiny__.attributes.texcoords[2 * idx.texcoord_index + 0];
										float ty = __tiny__.attributes.texcoords[2 * idx.texcoord_index + 1];
										_vt_.emplace_back(tx, ty, 1.0f);
									}
									else {
										_vt_.emplace_back(0.0f, 0.0f, -1.0f); // use the last coordinate to indicate valid texture
									}
									_ft_idx_.v[v] = _vt_.size() - 1;
								}								
							}
							_fx_.push_back(_fx_idx_);
							if (has_normals) { _fn_.push_back(_fn_idx_); }
							if (has_texcoords) { _ft_.push_back(_ft_idx_); }
							index_offset += fv;
							// per-face material
							int mtl_index = __tiny__.shapes[s].mesh.material_ids[f];
							if (mtl_index >= 0) {
								__mtlData__[mtl_index].push_back(_fx_.size() - 1);
							}
							else {
								__mtlData__.back().push_back(_fx_.size() - 1);
							}
						}
					}
					//
					// set attribute sizes
					SetNumFaces((unsigned int)_fx_.size());
					SetNumVertex((unsigned int)_vx_.size());
					SetNumNormals((unsigned int)_vn_.size());
					SetNumTexVerts((unsigned int)_vt_.size());
					if (loadMtl) SetNumMtls((unsigned int)__mtlData__.size());
					//
					// copy attribute data
					memcpy(v, _vx_.data(), sizeof(cy::Point3f) * _vx_.size());
					if (_vt_.size() > 0) memcpy(vt, _vt_.data(), sizeof(cy::Point3f)*_vt_.size());
					if (_vn_.size() > 0) memcpy(vn, _vn_.data(), sizeof(cy::Point3f)*_vn_.size());
					//
					// compute mcfc
					if (loadMtl && __mtlData__.size() > 0) {
						unsigned int fid = 0;
						for (int m = 0; m < (int)__mtlData__.size(); m++) {
							for (auto i = __mtlData__[m].begin(); i != __mtlData__[m].end(); i++) {
								f[fid] = _fx_[*i];
								if (fn) fn[fid] = _fn_[*i];
								if (ft) ft[fid] = _ft_[*i];
								fid++;
							}
							mcfc[m] = fid;
						}
					}
					else {
						memcpy(f, _fx_.data(), sizeof(TriFace) * _fx_.size());
						if (ft) memcpy(ft, _ft_.data(), sizeof(TriFace) * _ft_.size());
						if (fn) memcpy(fn, _fn_.data(), sizeof(TriFace) * _fn_.size());
					}
					//
					// Load the .mtl files
					if (loadMtl) { // use default value for the last material
						for (int m = 0; m < (int)__mtlData__.size() - 1; m++) 
						{
							// constant integers
							TriMesh::m[m].illum = __tiny__.materials[m].illum;
							TriMesh::m[m].Ns = __tiny__.materials[m].shininess;
							TriMesh::m[m].Ni = __tiny__.materials[m].ior;
							// copy vectors
							memcpy(TriMesh::m[m].Ka, __tiny__.materials[m].ambient, sizeof(float) * 3);
							memcpy(TriMesh::m[m].Kd, __tiny__.materials[m].diffuse, sizeof(float) * 3);
							memcpy(TriMesh::m[m].Ks, __tiny__.materials[m].specular, sizeof(float) * 3);
							memcpy(TriMesh::m[m].Tf, __tiny__.materials[m].transmittance, sizeof(float) * 3);
							// copy texture filename
							helper::copychar(TriMesh::m[m].name.data, __tiny__.materials[m].name);
							helper::copychar(TriMesh::m[m].map_d.data, __tiny__.materials[m].alpha_texname);
							helper::copychar(TriMesh::m[m].map_Ka.data, __tiny__.materials[m].ambient_texname); 
							helper::copychar(TriMesh::m[m].map_Kd.data, __tiny__.materials[m].diffuse_texname);
							helper::copychar(TriMesh::m[m].map_Ks.data, __tiny__.materials[m].specular_texname);
							helper::copychar(TriMesh::m[m].map_Ns.data, __tiny__.materials[m].specular_highlight_texname);
							helper::copychar(TriMesh::m[m].map_bump.data, __tiny__.materials[m].bump_texname);
							helper::copychar(TriMesh::m[m].map_disp.data, __tiny__.materials[m].displacement_texname);
						}
					}
					return true;
				}
			}
			else { return cy::TriMesh::LoadFromFileObj(fname, true); }
		}

		void ComputeArrayData() {
			for (auto i = 0; i < NF(); ++i) {
				auto vIdx = F(i);
				auto nIdx = HasNormals()         ? FN(i) : cy::TriMesh::TriFace();
				auto tIdx = HasTextureVertices() ? FT(i) : cy::TriMesh::TriFace();
				for (auto j = 0; j < 3; ++j) {
					auto v = vIdx.v[j];
					auto n = HasNormals()         ? nIdx.v[j] : -1;
					auto t = HasTextureVertices() ? tIdx.v[j] : -1;
					__arraydata__.vtptr.push_back(V(v));
					if (HasNormals()) { __arraydata__.nmptr.push_back(VN(n)); }
					if (HasTextureVertices()) { __arraydata__.txptr.push_back(VT(t)); }
				}
			}
		}

		void ComputeIndexData() {
			__indexdata__.nmcount = std::vector<int>(NV(), 0);
			__indexdata__.txcount = std::vector<int>(NV(), 0);
			for (auto i = 0; i < NF(); ++i) {
				auto vIdx = F(i);
				auto nIdx = HasNormals()         ? FN(i) : cy::TriMesh::TriFace();
				auto tIdx = HasTextureVertices() ? FT(i) : cy::TriMesh::TriFace();
				for (auto j = 0; j < 3; ++j) {
					auto v = vIdx.v[j];
					auto n = HasNormals()         ? nIdx.v[j] : -1;
					auto t = HasTextureVertices() ? tIdx.v[j] : -1;
					// compute for normal
					if (HasNormals()) {
						if (__indexdata__.nmcount[v] == 0) {
							__indexdata__.nmptr.push_back(VN(n));
						}
						else {
							__indexdata__.nmptr[v] =
								(__indexdata__.nmptr[v] * __indexdata__.nmcount[v] + VN(n)) / (__indexdata__.nmcount[v] + 1);
						}
						++__indexdata__.nmcount[v];
					}
					// compute for texture coordinate
					if (HasTextureVertices()) {
						if (__indexdata__.txcount[v] == 0) {
							__indexdata__.txptr.push_back(VT(n));
						}
						else {
							__indexdata__.txptr[v] =
								(__indexdata__.txptr[v] * __indexdata__.txcount[v] + VT(n)) / (__indexdata__.txcount[v] + 1);
						}
						++__indexdata__.txcount[v];
					}
				}
			}
			__indexdata__.nmcount.clear();
			__indexdata__.txcount.clear();
		}

		void GetArrayData(void* *vptr, void* *nptr, void* *tptr, void* *iptr, unsigned int *vsz, unsigned int *nsz, unsigned int *tsz,unsigned int *isz, unsigned int *num) {
			*num = NF() * 3;
			*vptr = __arraydata__.vtptr.data();
			*vsz = sizeof(cy::Point3f) * NF() * 3;
			*nptr = HasNormals() ? __arraydata__.nmptr.data() : nullptr;
			*nsz =  HasNormals() ? sizeof(cy::Point3f) * NF() * 3 : 0;
			*tptr = HasTextureVertices() ? __arraydata__.txptr.data() : nullptr;
			*tsz =  HasTextureVertices() ? sizeof(cy::Point3f) * NF() * 3 : 0;
			*iptr = NULL;
			*isz = 0;
		}

		void GetIndexData(void* *vptr, void* *nptr, void* *tptr, void* *iptr, unsigned int *vsz, unsigned int *nsz, unsigned int *tsz, unsigned int *isz, unsigned int *num) {
			*num = NV();
			*vptr = &(V(0));
			*vsz = sizeof(cy::Point3f) * NV();
			*nptr = HasNormals() ? __indexdata__.nmptr.data() : nullptr;
			*nsz = HasNormals() ? sizeof(cy::Point3f) * NV() : 0;
			*tptr = HasTextureVertices() ? __indexdata__.txptr.data() : nullptr;
			*tsz = HasTextureVertices() ? sizeof(cy::Point3f) * NV() : 0;
			*iptr = &(F(0));
			*isz = sizeof(cy::TriMesh::TriFace) * NF();
		}

		void GetBufferVertex(void* &vptr, unsigned int &vsz, bool use_element_draw = false) {
			if (use_element_draw) {
				vptr = &(V(0));
				vsz = sizeof(cy::Point3f) * NV();
			}
			else {
				vptr = __arraydata__.vtptr.data();
				vsz = sizeof(cy::Point3f) * NF() * 3;
			}
		}

		void GetBufferNormal(void* &nptr, unsigned int &nsz, bool use_element_draw = false) {
			if (use_element_draw) {
				nptr = HasNormals() ? __indexdata__.nmptr.data() : nullptr;
				nsz = HasNormals() ? sizeof(cy::Point3f) * NV() : 0;
			}
			else {
				nptr = HasNormals() ? __arraydata__.nmptr.data() : nullptr;
				nsz = HasNormals() ? sizeof(cy::Point3f) * NF() * 3 : 0;
			}
		}

		void GetBufferTexcoord(void* &tptr, unsigned int &tsz, bool use_element_draw = false) {
			if (use_element_draw) {
				tptr = HasTextureVertices() ? __indexdata__.txptr.data() : nullptr;
				tsz = HasTextureVertices() ? sizeof(cy::Point3f) * NV() : 0;
			}
			else {
				tptr = HasTextureVertices() ? __arraydata__.txptr.data() : nullptr;
				tsz = HasTextureVertices() ? sizeof(cy::Point3f) * NF() * 3 : 0;
			}
		}

		void GetBufferIndex(void* &iptr, unsigned int &isz, unsigned int &num, bool use_element_draw = false) {
			if (use_element_draw) {
				num = NV();
				iptr = &(F(0));
				isz = sizeof(cy::TriMesh::TriFace) * NF();
			}
			else {
				num = NF() * 3;
				iptr = NULL;
				isz = 0;
			}
		}

	};

};

#endif//_ICG_MESH_WRAPPER_H_
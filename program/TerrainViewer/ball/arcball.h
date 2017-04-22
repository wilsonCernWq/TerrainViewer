//!
//! This file defines the trackball class, which is independent to the project itself.
//!
#pragma once
#ifndef _ARCBALL_H_
#define _ARCBALL_H_

#include "../common/icg_common.h"
#include "ball.h"

namespace icg {
	//!
	//! following the implementation of https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball
	//!
	class Arcball : public Ball {
	private:
		float radius = 1.0f;
		bool flag_inverse = false;
		cy::Matrix4f* view;
		cy::Matrix4f matrix = cy::Matrix4f::MatrixIdentity();
		cy::Matrix4f matrix_prev = cy::Matrix4f::MatrixIdentity();
		float last_mx = 0.0f, last_my = 0.0f, cur_mx = 0.0f, cur_my = 0.0f;
	public:
		Arcball(cy::Matrix4f* v) { view = v; }

		void SetModeInverse(bool flag) { flag_inverse = flag; }

		void BeginDrag(float x, float y)
		{
			last_mx = cur_mx = x;
			last_my = cur_my = y;
			matrix_prev = matrix;
		}

		void Drag(float x, float y)
		{
			cur_mx = x;
			cur_my = y;
			if (cur_mx != last_mx || cur_my != last_my) {
				cy::Point3f va = get_arcball_vector(last_mx, last_my);
				cy::Point3f vb = get_arcball_vector(cur_mx, cur_my);
				float angle = acos(std::min(1.0f, va.Dot(vb)));
				cy::Point3f axis_in_camera_coord = va.Cross(vb).GetNormalized();
				cy::Matrix3f camera2object = view != nullptr ? (*view).GetSubMatrix3().GetInverse() : cy::Matrix3f::MatrixIdentity();
				cy::Point3f axis_in_object_coord = camera2object * axis_in_camera_coord;
				matrix = cy::Matrix4f::MatrixRotation(axis_in_object_coord, angle) * matrix_prev;
				last_mx = cur_mx;
				last_my = cur_my;
				matrix_prev = matrix;
			}
		}

		cy::Matrix4f Matrix()
		{
			return matrix;
		}

		void Reset() { matrix = cy::Matrix4f::MatrixIdentity(); }

	private:
		//! get_arcball_vector: project (x,y) mouse pos on surface
		//! @param x: X position
		//! @param y: Y position
		//! @return projected position
		cy::Point3f get_arcball_vector(float x, float y) {
			cy::Point3f P = cy::Point3f(x, y, 0);
			float OP_squared = P.x * P.x + P.y * P.y;
			if (OP_squared <= radius * radius) {
				P.z = sqrt(radius * radius - OP_squared);  // Pythagore
			}
			else {
				P.Normalize();  // nearest point
			}
			return P;
		}

	};
};

#endif//_ARCBALL_H_

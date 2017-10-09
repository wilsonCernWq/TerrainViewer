#pragma once

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "configure.h"

namespace icg {

	//! Camera object
	class Camera {
	public:		
		enum CameraMode { PERSPECTIVE, ORGHOGONAL };
	private:
		//! camera mode
		CameraMode mode = PERSPECTIVE;
		//! rotational matrix
		Ball *ball = nullptr;
		//! initial information
		cy::Point3f init_position = cy::Point3f(30.0f, 0.0f, 0.0f);
		cy::Point3f init_focus = cy::Point3f(0.0f, 0.0f, 0.0f);
		cy::Point3f init_up = cy::Point3f(0.0f, 1.0f, 0.0f);
		//! current information
		cy::Point3f current_position;
		cy::Point3f current_focus;
		cy::Point3f current_up;
		//! view matrix (world -> camera coordinate)
		cy::Matrix4f view = cy::Matrix4f::MatrixIdentity();
		cy::Matrix4f proj = cy::Matrix4f::MatrixIdentity();	
		//! viewport
		float aspect, halfwidth, halfheight, znear, zfar;
		bool fixaspect;
	public:
		void PrintSelf(std::ostream& out, int indentation = 0) {
			out << std::string(indentation, ' ') << "Current World Coordinate Position: "
				<< "(" << current_position.x << ", " << current_position.y << ", " << current_position.z << ")" << std::endl;
			out << std::string(indentation, ' ') << "Current World Coordinate Focus"
				<< "(" << current_focus.x << ", " << current_focus.y << ", " << current_focus.z << ")" << std::endl;
			out << std::string(indentation, ' ') << "Current World Coordinate Up: "
				<< "(" << current_up.x << ", " << current_up.y << ", " << current_up.z << ")" << std::endl;
		}
		//!
		//! Constructor
		Camera(CameraMode mode = PERSPECTIVE) { 
			this->mode = mode;
			this->ball = new Trackball();
		}
		//!
		//! Destructor
		~Camera() { delete ball; }
		//!
		//! update camera current position
		void UpdateCamera() {
			this->current_position = (this->init_focus - this->init_position) * this->ball->ZoomRatio() + this->init_position;
			this->current_position = (this->ball->Matrix() * cy::Point4f(this->current_position, 1.0f)).GetNonHomogeneous();
			this->current_focus = this->init_focus;
			this->current_up = cy::Point3f(this->ball->Matrix() * cy::Point4f(this->init_up, 0.0f));
		}
		void UpdateView() {
			this->view = LookAt(this->current_position, this->current_focus, this->current_up);
		}
		void UpdateProjection(float aspect, float x, float y, float n, float f, bool fixaspect = false) {
			this->aspect = aspect;
			this->halfwidth = x;
			this->halfheight = y;
			this->znear = n;
			this->zfar = f;
			this->fixaspect = fixaspect;
			if (this->mode == PERSPECTIVE) {
				this->proj = PerspectiveProj(aspect, x, y, n, f, fixaspect);
			}
			else if (this->mode == ORGHOGONAL) {
				this->proj = OrthographicProj(aspect, x, y, n, f, fixaspect);
			}
		}
		//!
		//! get
		icg::Ball* GetBall() { return this->ball; }
		cy::Matrix4f& GetView() {
			return this->view;
		}
		cy::Matrix4f& GetProjection() {
			return this->proj;
		}
		cy::Point3f& GetCurrentPosition() { return this->current_position; }
		cy::Point3f& GetInitPosition() { return this->init_position; }
		float GetNearClip() { return znear; }
		float GetFarClip() { return zfar; }
		//!
		//!
		void SetInitPosition(cy::Point3f position) {
			this->init_position = position;
		}
		void SetInitFocus(cy::Point3f focus) {
			this->init_focus = focus;
		}
		void SetInitUp(cy::Point3f up) {
			this->init_up = up;
		}

	};

};

#endif//_CAMERA_H_

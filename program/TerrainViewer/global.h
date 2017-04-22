#pragma once
#ifndef _GLOBALVAR_H_
#define _GLOBALVAR_H_

#include "configure.h"

namespace sysinfo {

	// flags
	enum { ROTATE, ZOOM } RotateMode;

	// window information
	unsigned int WINX = 0, WINY = 0, WIDTH = 800, HEIGHT = 800;

	// background color
	bool bgchange = false;
	float bgcolor[] = { 0.0, 0.0, 0.5, 0, 0, 0 };

	// fps debugging
	icg::FrameCounter fps;
	std::stringstream title;

};

namespace model {

	//! camera and light
	icg::Camera cam_main(icg::Camera::CameraMode::PERSPECTIVE);
	icg::Camera* camera = &cam_main;
	icg::LightObject light;
	icg::TerrainObject terrain;
	icg::NoiseObject noise;

};

#endif//_GLOBALVAR_H_

/**
 * This is the header of every file. It includes all external libraries and defines all basic types
 */
#pragma once
#ifndef _ICG_COMMON_H_
#define _ICG_COMMON_H_

#define NOMINMAX

//
// include cpp standard library
//
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>

#include <GL/glew.h>
#ifdef __APPLE__ // apple specific header
#   include <OpenGL/gl.h>
#	include <OpenGL/glu.h>
#	include <GLUT/glut.h>
#else
#	if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__) //	Windows need special header
#		include <Windows.h>
#	else
#		if __unix__	// Linux needs extensions for framebuffers
#			define GL_GLEXT_PROTOTYPES 1
#			include <GL/glext.h>
#		endif
#	endif
#	include <GL/gl.h>
#	include <GL/glu.h>
#	include <GL/glut.h>
#	include <GL/freeglut.h>
#endif

//
// include cyCodeBase here
//
#include <cyCore.h>
#include <cyTimer.h>
#include <cyPoint.h>
#include <cyTriMesh.h>
#include <cyMatrix.h>
#include <cyGL.h>

//
// include LodePNG
//
#include "lodepng.h"

//
// imgui
//
#include "imgui.h"
#include "../gui/imgui_impl_glut.h"

#if WITH_EIGEN
// optional
#endif

#if WITH_GLM
// optional
#endif

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

// global debug flag
bool GLDEBUG = false;

#endif//_ICG_COMMON_H_

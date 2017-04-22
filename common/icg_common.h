// Copyright (C) 2014 - LGG EPFL
#pragma once

///--- Load OpenGL here (cross platform)
#include <GL/glew.h> ///< must be before glfw
#include <GL/glfw.h>

///--- Linux needs extensions for framebuffers
#if __unix__
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glext.h>
#endif

///--- Standard library IO
#include <iostream>
#include <cassert>

///--- We use Eigen for linear algebra
#include <Eigen/Dense>
typedef Eigen::Vector2f vec2;
typedef Eigen::Vector3f vec3;
typedef Eigen::Vector4f vec4;
typedef Eigen::Matrix4f mat4;
typedef Eigen::Matrix3f mat3;
#include <OpenGP/GL/EigenOpenGLSupport3.h>
/// We use a modified (to support OpenGL3) version of the Eigen OpenGL module 
/// @see http://eigen.tuxfamily.org/dox/unsupported/group__OpenGLSUpport__Module.html
// #include <Eigen/OpenGL3Support>

/// On some OSs the exit flags are not defined
#ifndef EXIT_SUCCESS
    #define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
    #define EXIT_FAILURE 1
#endif

///--- Shader compiling utilities
#include "OpenGP/GL/shader_helpers.h"

///--- Utilities to simplify glfw setup
#include <OpenGP/GL/glfw_helpers.h>

///--- For mesh I/O we use OpenGP
#include <OpenGP/Surface_mesh.h>

///--- GL Error checking
#include "check_error_gl.h"

///--- These namespaces assumed by default
using namespace std;
using namespace opengp;

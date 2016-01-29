#ifndef _ICG_GLOBAL_H_
#define _ICG_GLOBAL_H_
#include "icg_library.h"

/**
 * @brief To define global variables
 */

namespace glob
{
/** @var constants */
static int WIDTH  = 1280;
static int HEIGHT = 720;

/** @var camera coordinates */
static vec3 campos;
static vec3 camdir;
static vec3 camupv;
static vec2 origin;

/** @var transformation matrices */
static mat4 lview;
static mat4 lproj;
static mat4 vview;
static mat4 vproj;
static Transform model;
static Trackball tball;

/** @var flow control */
enum NAVIGATION_MODE { FPS, FREECAM };
static NAVIGATION_MODE navmode = FREECAM;

/** @var global objects */
Light sun;
FrameBuffer depth(2048, 2048);
Terrain terrain(sun, lproj, lview, origin);
Water water(sun, terrain.sandH, terrain.getMaxScales());
}

#endif // _ICG_GLOBAL_H_

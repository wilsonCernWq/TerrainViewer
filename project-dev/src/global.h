#ifndef GLOBAL_H
#define GLOBAL_H
#include "icglib.h"
#include "../lib/Trackball.h"
/**
 * To define global constants
 */
namespace glob {
int WIDTH = 1280;
int HEIGHT = 720;
vec3 campos = vec3(0.0, 5.0, 10.0);
vec3 camdir = vec3(0.0, 0.0,-1.0);
vec3 camupv = vec3(0.0, 1.0, 0.0);
vec2 origin = vec2(0.0,0.0);
mat4 ligview;
mat4 ligproj;
mat4 view;
mat4 proj;
Trackball trackball;
}
#endif

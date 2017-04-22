#ifndef ICGPROJECT_H
#define ICGPROJECT_H
#include "icg_common.h"
/** Class & Functions **/
#include "../lib/trackball.h"
#include "../lib/Noise.h"

/** Basic Classes **/
#include "../lib/Light.h"
#include "../lib/Terrain.h"
#include "../lib/quad/Quad.h"
#include "../lib/sky/Sky.h"
#include "../lib/waterquad/Water.h"
#include "../lib/screenquad/ScreenQuad.h"
#include "../lib/FrameBuffer.h"

typedef Eigen::Transform<float,3,Eigen::Affine> Transform;

#endif

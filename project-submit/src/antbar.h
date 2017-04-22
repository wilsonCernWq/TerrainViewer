#ifndef ANTBAR_H
#define ANTBAR_H
#include "icg_common.h"
#include "icglib.h"
#include "global.h"
#include "../lib/waterquad/Water.h"
#include "../lib/bezier/bezier.h"
#ifdef WITH_ANTTWEAKBAR
#include <AntTweakBar.h>
TwBar *bar;
#endif

using namespace glob;
extern Terrain terrain;
extern Light sun;
extern Water water;
extern BezierCurve camera_path;

void init_bar(){
#ifdef WITH_ANTTWEAKBAR
    TwInit(TW_OPENGL_CORE, NULL);
    TwWindowSize(WIDTH, HEIGHT);
    bar = TwNewBar("AntTweakBar");
    /// @todo Terrain values
    TwAddSeparator(bar, "Terrain", NULL);
    TwAddVarRW(bar, "max height", TW_TYPE_FLOAT, &terrain.maxHeight, "min=.1 max=5.0 step=0.1");
    TwAddVarRW(bar, "display ratio", TW_TYPE_FLOAT, &terrain.displayRatio, "min=.1 max=5.0 step=0.1");
    TwAddVarRW(bar, "display range", TW_TYPE_FLOAT, &terrain.terrainRange, "min=1.0 max=20.0 step=1.0");
    //TwAddVarRW(bar, "water height", TW_TYPE_FLOAT, &water.water_height, "min=.1 max=5.0 step=0.01");
    /// @todo Light
    TwAddSeparator(bar, "Light", NULL);
    TwAddVarRW(bar, "LightDir", TW_TYPE_DIR3F, &sun.light_pos, "");
    TwAddVarRW(bar, "Id", TW_TYPE_COLOR3F, sun.Id.data(), NULL);
    /// @todo Camera
    TwAddSeparator(bar, "Camera", NULL);
    TwAddVarRW(bar, "CameraPos", TW_TYPE_DIR3F, &campos, "");
    TwAddVarRW(bar, "CameraDir", TW_TYPE_DIR3F, &camdir, "");
    TwAddVarRW(bar, "speed", TW_TYPE_INT16, &path_speed, "min=1 max=100 step=1");
    std::vector<vec3*> points = camera_path.getPoints();
    for (int i = 0; i < points.size(); i++) {
        string name = "point "+std::to_string(i+1);
        TwAddVarRW(bar, name.c_str(), TW_TYPE_DIR3F, points.at(i), "");
    }
#endif
}

void draw_bar(){
#ifdef WITH_ANTTWEAKBAR
    TwDraw();
#endif
}

void clean_bar(){
#ifdef WITH_ANTTWEAKBAR
    TwTerminate();
#endif
}

#endif

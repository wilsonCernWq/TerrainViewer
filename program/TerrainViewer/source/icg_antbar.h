#ifndef _ICG_ANTBAR_H_
#define _ICG_ANTBAR_H_

/** @brief Wrapper for AntTweakBar */

#include "icg_library.h"
#include "icg_global.h"

#ifdef WITH_ANTTWEAKBAR
#include <AntTweakBar.h>
TwBar *bar;
#endif

inline void init_bar()
{
#ifdef WITH_ANTTWEAKBAR
    using namespace glob;
    TwInit(TW_OPENGL_CORE, NULL);
    TwWindowSize(glob::WIDTH, glob::HEIGHT);
    bar = TwNewBar("AntTweakBar");

    extern Terrain terrain;
    extern Light sun;

    /// @todo Terrain values
    TwAddSeparator(bar, "Terrain", NULL);
    TwAddVarRW(bar, "maximum height", TW_TYPE_FLOAT, &terrain.maxH, "min=0.0 max=20.0 step=0.1");
    TwAddVarRW(bar, "display ranges", TW_TYPE_FLOAT, &terrain.terR, "min=1.0 max=20.0 step=1.0");
    TwAddVarRW(bar, "sand level", TW_TYPE_FLOAT, &terrain.sandH, "min=-10.0 max=10.0 step=0.01");
    TwAddVarRW(bar, "snow level", TW_TYPE_FLOAT, &terrain.snowH, "min=-10.0 max=10.0 step=0.01");

    TwAddVarRW(bar, "H", TW_TYPE_FLOAT, &terrain.H, "min= 0.0 max= 2.0  step=0.01 ");
    TwAddVarRW(bar, "L", TW_TYPE_FLOAT, &terrain.L, "min= 1.0 max= 8.0  step=0.01 ");
    TwAddVarRW(bar, "O", TW_TYPE_FLOAT, &terrain.O, "min= 0.0 max=20.0  step=1.00 ");
    TwAddVarRW(bar, "G", TW_TYPE_FLOAT, &terrain.G, "min= 0.0 max=10.0  step=0.01 ");
    TwAddVarRW(bar, "B", TW_TYPE_FLOAT, &terrain.B, "min= 0.0 max= 0.5  step=0.01 ");

    /// @todo Light
    TwAddSeparator(bar, "Light", NULL);
    TwAddVarRW(bar, "LightDir", TW_TYPE_DIR3F, &sun.light_position, "");
    TwAddVarRW(bar, "Ia", TW_TYPE_COLOR3F, sun.Ia.data(), NULL);
    TwAddVarRW(bar, "Id", TW_TYPE_COLOR3F, sun.Id.data(), NULL);
    TwAddVarRW(bar, "Is", TW_TYPE_COLOR3F, sun.Is.data(), NULL);

    /// @todo Camera
    //TwAddSeparator(bar, "Camera", NULL);
    //TwAddVarRW(bar, "CameraPos", TW_TYPE_DIR3F, &campos, "");
    //TwAddVarRW(bar, "CameraDir", TW_TYPE_DIR3F, &camdir, "");

#endif
}

inline void draw_bar(){
#ifdef WITH_ANTTWEAKBAR
    TwDraw();
#endif
}

inline void clean_bar(){
#ifdef WITH_ANTTWEAKBAR
    TwTerminate();
#endif
}
#endif // _ICG_ANTBAR_H_

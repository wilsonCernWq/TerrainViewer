#include "icglib.h"
#include "global.h"
#include "antbar.h"
#include "icgfun.h"
///////////////////////////////////////////////////////////////
/// @brief Global constants definition
///////////////////////////////////////////////////////////////
using namespace glob;
//Sky sky;
Light sun;
Water water;
Terrain terrain(sun, glob::ligproj, glob::ligview, glob::origin);
ScreenQuad screen;
///////////////////////////////////////////////////////////////
FrameBuffer rewat(WIDTH, HEIGHT);
FrameBuffer depth(2048, 2048);
FrameBuffer fogde(2048, 2048);
FrameBuffer color(WIDTH, HEIGHT);
Transform Model;
/**
 * @brief init: program initialization
 */
void init() {
    //////////////////////////////////////////////////////////////////////////
    /// @todo System setup
    //////////////////////////////////////////////////////////////////////////
    glClearColor(1, 1, 1, /*solid*/1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    //////////////////////////////////////////////////////////////////////////
    /// @todo initialize Global 3D Transformation parameters
    //////////////////////////////////////////////////////////////////////////
    glob::proj = Eigen::perspective(50.0f, (float)WIDTH / (float)HEIGHT, 0.01f, 1000.0f);;
    glob::ligproj = OrthographicProjection(-20, 20, -20, 20, -20, 60);
    //////////////////////////////////////////////////////////////////////////
    /// @todo Initialization for terrain draw
    //////////////////////////////////////////////////////////////////////////
    //*/ For shadow map
    GLuint depth_buffer = depth.init(0)[0];
    GLuint color_buffer = color.init(1)[0];
    screen.setTexture(color_buffer);
    screen.setZbuffer(fogde.init(0)[0]);
    terrain.init(depth_buffer);
    screen.init();
    //*/
    //*/ Water
    water.setLight(sun);
    water.setTexture(rewat.init(1)[0]);
    water.init();
    //sky.init();
    //*/
    Model = Eigen::AlignedScaling3f(50.0, 50.0, 50.0);
    Model *= Eigen::AngleAxisf(-0.5 * M_PI, Eigen::Vector3f::UnitX());
    Model *= Eigen::Translation3f(0,0,0);
    //////////////////////////////////////////////////////////////////////////
    /// @todo AntTweakBar initialization & debug
    //////////////////////////////////////////////////////////////////////////
    init_bar();
    check_error_gl();
}

/**
 * @brief display: actions in each Main Loop
 */
void display() {
    //////////////////////////////////////////////////////////////////////////
    /// @todo Setup camera
    //////////////////////////////////////////////////////////////////////////
    move_camera();
    campos(1) = terrain.getTerrainHeight(campos(0), campos(2), 50.0) + 1.0;
    glob::view = Eigen::lookAt<vec3>(glob::campos, glob::campos + glob::camdir, glob::camupv);
    //////////////////////////////////////////////////////////////////////////
    /// @todo Setup light
    //////////////////////////////////////////////////////////////////////////
    vec3 ligPos = vec3(sun.light_pos(0), sun.light_pos(2), -sun.light_pos(1));
    glob::ligview = Eigen::lookAt<vec3>(ligPos, vec3(0,0,0), vec3(0,1,0));
    //////////////////////////////////////////////////////////////////////////
    /// @todo Draw
    //////////////////////////////////////////////////////////////////////////
    glViewport(0,0,WIDTH,HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
        mat4 M = Model.matrix();
        //*/ Get depth map
        depth.bind();
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            terrain.drawDepth(M, origin);
        }
        depth.unbind();
        fogde.bind();
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            terrain.drawFog(M, glob::view, glob::proj, trackball.matrix(), origin);
            //sky.drawfog(glob::proj * glob::view * trackball.matrix(), glfwGetTime());
        }
        fogde.unbind();
        //*/
        water.setWaterHeight(terrain.getWaterHeight());
        //*/ Get the reflection for the water
        rewat.bind(0);
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            terrain.draw(M, glob::view, glob::proj, trackball.matrix(), origin);
        }
        rewat.unbind();
        //*/

        //*/ Get color map
        color.bind();
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            terrain.draw(M, glob::view, glob::proj, trackball.matrix(), origin);
            water.draw(trackball.matrix() * M, view, glob::proj);
            //sky.draw(glob::proj * glob::view * trackball.matrix(), glfwGetTime());
        }
        color.unbind();
        //*/
        //*/
        glViewport(0,0,WIDTH,HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        screen.draw();
        //*/
    }
    //////////////////////////////////////////////////////////////////////////
    /// @todo AntTweakBar draw & debug
    //////////////////////////////////////////////////////////////////////////
    draw_bar();
    check_error_gl();
}

/**
 * @brief cleanup: cleaning every thing
 */
void cleanup(){}

/**
 * @brief main: Program entrance
 * @return Terminal status
 */
int main(int, char**) {
    glfwInitWindowSize(WIDTH, HEIGHT);
    glfwCreateWindow("icg-project");
    /// @todo Specify Main Loop function
    glfwDisplayFunc(display);
    /// @todo Setup ballback functions
    glfwSetWindowSizeCallback(&resize_callback);
    glfwSetMouseButtonCallback(mouse_button);
    glfwSetMousePosCallback(mouse_pos);
    glfwSetMouseWheelCallback(Mouse_Wheel);
    glfwSetKeyCallback(keyboard);
    init();
    /// @todo Start Main Loop
    glfwMainLoop();
    return EXIT_SUCCESS;    
}

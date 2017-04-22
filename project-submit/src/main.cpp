#include "icglib.h"
#include "global.h"
#include "antbar.h"
#include "icgfun.h"
///////////////////////////////////////////////////////////////
/// @brief Global constants definition
///////////////////////////////////////////////////////////////
using namespace glob;
Sky sky;
Light sun;
Water water;
Terrain terrain(sun, glob::ligproj, glob::ligview, glob::origin);
ScreenQuad screen;
///////////////////////////////////////////////////////////////
FrameBuffer waterReflexion(WIDTH, HEIGHT); //< Framebuffer for water
FrameBuffer depth(2048, 2048);
FrameBuffer color(WIDTH, HEIGHT);
Transform Model;
BezierCurve camera_path;
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //////////////////////////////////////////////////////////////////////////
    /// @todo initialize Global 3D Transformation parameters
    //////////////////////////////////////////////////////////////////////////
    glob::proj = Eigen::perspective(50.0f, (float)WIDTH / (float)HEIGHT, 0.01f, 1000.0f);;
    glob::ligproj = OrthographicProjection(-100, 100, -100, 100, -100, 100);
    //////////////////////////////////////////////////////////////////////////
    /// @todo Initialization for terrain draw
    //////////////////////////////////////////////////////////////////////////
    //*/ For shadow map
    GLuint depth_buffer = depth.init(0)[0];
    GLuint color_buffer = color.init(1)[0];
    screen.setTexture(color_buffer);
    screen.setZbuffer(depth_buffer);
    terrain.setWaterHeight(&water.water_height);
    terrain.init(depth_buffer);
    screen.init();
    //*/
    //*/ Water
    water.setLight(sun);
    water.setTexture(waterReflexion.init(1)[0]);
    water.setWaterHeight(0.2);
    water.init();
    sky.init();
    //*/
    Model = Eigen::AlignedScaling3f(50.0, 50.0, 50.0);
    Model *= Eigen::AngleAxisf(-0.5 * M_PI, Eigen::Vector3f::UnitX());
    Model *= Eigen::Translation3f(0,0,0);

    //* Camera path
    camera_path.addSegment(vec3(0.8, 0.8, 1), vec3(0.8, -0.8, 1), vec3(0.8, -0.8, 1), vec3(-0.8, -0.5, 1));
    camera_path.addSegment(vec3(0, 0, 0), vec3(-0.8, 0.8, 1), vec3(0, 0.8, 1), vec3(0.8, 0.8, 1));
    camera_path.init();
    //*/
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
    mat4 M = Model.matrix();
    vec3 ligPos = vec3(sun.light_pos(0), sun.light_pos(2), -sun.light_pos(1));
    glob::ligview = Eigen::lookAt<vec3>(ligPos, vec3(0,0,0), vec3(0,1,0));
    //////////////////////////////////////////////////////////////////////////
    if (navmode == BEZIER) {
        //////////////////////////////////////////////////////////////////////////
        vec3 cam_pos(0.0f, 0.0f, 0.0f);
        camera_path.sample_point(glfwGetTime(), path_speed, cam_pos);
        cam_pos = 10 * vec3(cam_pos(0), cam_pos(2), cam_pos(1));
        glob::view = Eigen::lookAt<vec3>(cam_pos, vec3(0, 0, 0), vec3(0,1,0));
        //////////////////////////////////////////////////////////////////////////
        glViewport(0,0,WIDTH,HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        {
            //*/ Get depth map
            depth.bind();
            {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                terrain.drawDepth(M, origin);
            }
            depth.unbind();
            //*/
            //*/ Get the reflection for the water
            waterReflexion.bind();
            {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                terrain.draw(M, glob::view, glob::proj, trackball.matrix(), origin, true);
            }
            waterReflexion.unbind();
            //*/
            //*/ Get color map
            {
                glViewport(0,0,WIDTH,HEIGHT);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                terrain.draw(M, glob::view, glob::proj, trackball.matrix(), origin);
                water.draw(trackball.matrix() * M, view, glob::proj);
                sky.draw(glob::proj * glob::view * trackball.matrix(), glfwGetTime());
            }
            //*/
        }
    } else {
        //////////////////////////////////////////////////////////////////////////
        move_camera(navmode);
        if (navmode != FREECAM) {
            campos(1) = terrain.getTerrainHeight(campos(0), campos(2), 50.0) + 1.0;
        }
        glob::view = Eigen::lookAt<vec3>(glob::campos, glob::campos + glob::camdir, glob::camupv);
        //////////////////////////////////////////////////////////////////////////
        glViewport(0,0,WIDTH,HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        {
            //*/ Get depth map
            depth.bind();
            {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                terrain.drawDepth(M, origin);
            }
            depth.unbind();
            //*/
            //*/ Get the reflection for the water
            waterReflexion.bind();
            {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                terrain.draw(M, glob::view, glob::proj, trackball.matrix(), origin, true);
            }
            waterReflexion.unbind();
            //*/
            //*/ Get color map
            {
                glViewport(0,0,WIDTH,HEIGHT);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                terrain.draw(M, glob::view, glob::proj, trackball.matrix(), origin);
                water.draw(trackball.matrix() * M, view, glob::proj);
                sky.draw(glob::proj * glob::view * trackball.matrix(), glfwGetTime());
            }
            //*/
            /* Camera path
            {
                camera_path.draw(trackball.matrix() * M, view, glob::proj);
            }
            //*/
        }
    }
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

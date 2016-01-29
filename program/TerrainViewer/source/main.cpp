#include "icg_library.h"
#include "icg_global.h"
#include "icg_antbar.h"
#include "icg_callback.h"

/**
 * @brief init: program initialization
 */
void init()
{
    using namespace glob;
    /** @todo System setup */
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    /** @todo initialize Global 3D Transformation parameters */
    reset_came();
    reset_proj();

    //*/ --- Modeling Matrix
    model  = Eigen::AlignedScaling3f(5.0f, 5.0f, 5.0f);
    model *= Eigen::AngleAxisf(-0.5f * M_PI, Eigen::Vector3f::UnitX());
    model *= Eigen::Translation3f(0.0f,0.0f,0.0f);
    //*/

    /** @todo Initialization for terrain draw */
    //*/ --- For shadow map
    terrain.init(depth.initDepth()[0]);
    //*/

    //*/
    water.init();
    //*/
    /** @todo AntTweakBar initialization & debug */
    init_bar();
    check_error_gl();
}

/**
 * @brief display: actions in each Main Loop
 */
void display()
{
    update_title_fps("icg-TerrainViewer");
    using namespace glob;
    //////////////////////////////////////////////////////////////////////////
    move_camera(navmode);
    vview = Eigen::lookAt<vec3>(campos, campos + camdir, camupv);
    lview = Eigen::lookAt<vec3>(sun.shadow_map(), vec3(0,0,0), vec3(0,1,0));
    //////////////////////////////////////////////////////////////////////////
    glViewport(0,0,WIDTH,HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
        //*/ Get depth map
        depth.bind();
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            terrain.drawDepth(model.matrix());
        }
        depth.unbind();
        //*/

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        terrain.draw(model.matrix(), vview, vproj, tball.matrix());
        water.draw(tball.matrix() * model.matrix(), vview, vproj);
    }
    draw_bar();
    check_error_gl();
}

/**
 * @brief cleanup: cleaning every thing
 */
void cleanup(){ clean_bar(); }

/**
 * @brief main: Program entrance
 * @return Terminal status
 */
int main(int, char**) {
    using namespace glob;
    /// Open window
    glfwInitWindowSize(WIDTH, HEIGHT);
    glfwCreateWindow("icg-TerrainViewer");
    glfwDisplayFunc(display);
    /// Setup ballback functions    
    glfwSetWindowSizeCallback(&resize_callback);
    glfwSetMouseButtonCallback(mouse_button);
    glfwSetMousePosCallback(mouse_pos);
    glfwSetMouseWheelCallback(mouse_wheel);
    glfwSetKeyCallback(keyboard);
    init();
    glfwMainLoop();
    //cleanup();
    return EXIT_SUCCESS;    
}

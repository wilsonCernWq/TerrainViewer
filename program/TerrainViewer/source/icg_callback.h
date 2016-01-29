#ifndef _ICG_CALLBACK_H_
#define _ICG_CALLBACK_H_
#include "icg_library.h"
#include "icg_global.h"
#include "icg_antbar.h"

/**
 * @brief resetcam: reset camera properties
 */
void reset_came()
{
    using namespace glob;
    tball.matrix() = mat4::Identity();    
    camupv = vec3(0.0f, 1.0f, 0.0f);
    origin = vec2(0.0f, 0.0f);
    if (navmode == FREECAM) {
        campos = vec3(100.0f, 100.0f, 100.0f);
        camdir = vec3(-1.0f, -1.0f, -1.0f).normalized();
    } else if (navmode == FPS) {
        campos = vec3(0.0f, 40.0f, 0.0f);
        camdir = vec3(0.0f,  0.0f,-1.0f);
    }
}

void reset_proj() {
    using namespace glob;
    vproj = PerspectiveProjection (50.0f, (float)WIDTH/(float)HEIGHT, 10.0f, 300.0f);
    lproj = OrthographicProjection(500.0f, 500.0f, -500.0f, 500.0f);
}

/**
 * @brief move_camera: Function to move camera position under different mode
 * @param mode
 */
void move_camera(glob::NAVIGATION_MODE mode)
{
    using namespace glob;
    if (glfwGetKey(GLFW_KEY_UP   ) == GLFW_PRESS) { origin(1) += (float)0.0001; }
    if (glfwGetKey(GLFW_KEY_DOWN ) == GLFW_PRESS) { origin(1) -= (float)0.0001; }
    if (glfwGetKey(GLFW_KEY_LEFT ) == GLFW_PRESS) { origin(0) -= (float)0.0001; }
    if (glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS) { origin(0) += (float)0.0001; }
}

/**
 * @brief resize_callback: resize action handling function
 * @param width: resized width
 * @param height: resized height
 */
void GLFWCALL resize_callback(int width, int height) {
    using namespace glob;
    WIDTH  = width;
    HEIGHT = height;
    glViewport(0, 0, width, height);
    reset_proj();
}

/**
 * @brief mouse_button: Mouse button handling function
 * @param button: Button index
 * @param action: Button action
 */
void GLFWCALL mouse_button(int button, int action) {
    using namespace glob;
    static vec2 p;
    static int  xpos;
    static int  ypos;
    if (TwEventMouseButtonGLFW(button, action)) { return; }
    if (navmode != FREECAM) { return; }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        glfwGetMousePos(&xpos, &ypos);
        icgScreen(xpos, ypos, WIDTH, HEIGHT, p);
        tball.begin_drag(p.x(), p.y());
    }
}

/**
 * @brief mouse_pos: Mouse position callbacl handling function
 * @param x
 * @param y
 */
void GLFWCALL mouse_pos(int xpos, int ypos) {
    using namespace glob;
    static vec2 p;
    if (TwEventMousePosGLFW(xpos, ypos)) { return; }
    icgScreen(xpos, ypos, WIDTH, HEIGHT, p);
    if (navmode == FREECAM)
    {
        if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            tball.drag(p(0), p(1));
        }
    }
    else if (navmode == FPS)
    {
        camdir = vec3(p(0), p(1), camdir(2)).normalized();
    }
}

/**
 * @brief Mouse_Wheel Mouse Wheel callbacl handling function
 * @param pos
 */
void GLFWCALL mouse_wheel(int pos){
    using namespace glob;
    static float record = 0.0f;
    if (navmode == FREECAM)
    {
        campos = campos + ((float) pos - record) * camdir;
    }
    else if (navmode == FPS)
    {
        vec3 movement = ((float)pos - record) * camdir;
        campos(1) = campos(1) + movement(1);
        origin(0) +=  0.0001 * movement(0);
        origin(1) += -0.0001 * movement(2);
    }
    record = pos;
}

/**
 * @brief keyboard: keyboard function handling
 * @param key: key type
 * @param action: key action
 */
void GLFWCALL keyboard(int key, int action){
    using namespace std;
    using namespace glob;
    if(action != GLFW_RELEASE) {
        switch(key){
        case GLFW_KEY_SPACE:
            glfwTerminate();
            cout << "---User Terminates---" << endl;
            break;
        case '1':
            navmode = FPS;
            reset_came();
            glfwDisable(GLFW_MOUSE_CURSOR);
            cout << "First Person Shot mode.\n" << flush;
            break;
        case '2':
            navmode = FREECAM;
            reset_came();
            glfwEnable(GLFW_MOUSE_CURSOR);
            cout << "Free Camera mode.\n" << flush;
            break;

        default:
            break;
        }
    }
}

#endif // _ICG_CALLBACK_H_

#ifndef ICGFUNC_H
#define ICGFUNC_H
#include <cmath>
#include "global.h"
#include "icglib.h"
#include "antbar.h"
using namespace glob;
static float time_prev = 0.0;
static float move_velo = 0.0;
static const float max_velo = 10.0;
/**
 * @brief transform_screen_coords: Transform screen position into world coordinates
 * @param x: x coordinate
 * @param y: y coordinate
 * @return Corresponding world coordinates
 */
vec2 transform_screen_coords(int x, int y) {
    return vec2(2.0f * (float)x / WIDTH - 1.0f, 1.0f - 2.0f * (float)y / HEIGHT);
}

/**
 * @brief LookAt Setup view matrix
 * @param eye Camera position
 * @param center Looking center
 * @param up Up vector
 * @return
 */
mat4 LookAt(vec3 eye, vec3 view, vec3 up) {
    vec3 z_cam = view.normalized();
    vec3 x_cam = up.cross(z_cam).normalized();
    vec3 y_cam = z_cam.cross(x_cam);

    mat3 R;
    R.col(0) = x_cam;
    R.col(1) = y_cam;
    R.col(2) = z_cam;

    mat4 look_at = mat4::Zero();
    look_at.block(0, 0, 3, 3) = R.transpose();
    look_at(3, 3) = 1.0f;
    look_at.block(0, 3, 3, 1) = -R.transpose() * (eye);
    return look_at;
}

/**
 * @brief OrthographicProjection: Calculate orthographic projection matrix
 * @param left: left boundary
 * @param right: right boundary
 * @param bottom: bottom boundary
 * @param top: top boundary
 * @param near: near boundary
 * @param far: far boundary
 * @return 4X4 orthographic projection matrix
 */
mat4 OrthographicProjection(float left, float right, float bottom, float top, float near, float far){
    assert(right > left);
    assert(top > bottom);
    assert(far > near);
    mat4 ortho = mat4::Zero();
    ortho(0, 0) = 2.0f / (right - left);
    ortho(1, 1) = 2.0f / (top - bottom);
    ortho(2, 2) = -2.0f / (far - near);
    ortho(3, 3) = 1.0f;
    ortho(1, 3) = -(right + left) / (right - left);
    ortho(2, 3) = -(top + bottom) / (top - bottom);
    ortho(2, 3) = -(far + near) / (far - near);
    return ortho;
}

/**
 * @brief PerspectiveProjection: Calculate perspective projection matrix
 * @param left: left boundary
 * @param right: right boundary
 * @param bottom: bottom boundary
 * @param top: top boundary
 * @param near: near boundary
 * @param far: far boundary
 * @return 4X4 perspective projection matrix
 */
mat4 PerspectiveProjection(float left, float right, float bottom, float top, float near, float far) {
    assert(right > left);
    assert(top > bottom);
    assert(far > near);
    mat4 frust = mat4::Zero();
    frust(0, 0) = 2.0f * near / (right - left);
    frust(1, 1) = 2.0f * near / (top - bottom);
    frust(2, 3) = -2.0f * far * near / (far - near);
    frust(2, 2) = -(far + near) / (far - near);
    frust(3, 2) = -1.0f;
    frust(0, 2) = (right + left) / (right - left);
    frust(1, 2) = (top + bottom) / (top - bottom);
    return frust;
}

/**
 * @brief resize_callback: resize action handling function
 * @param width: resized width
 * @param height: resized height
 */
void GLFWCALL resize_callback(int width, int height) {
    glViewport(0, 0, width, height);
    glob::WIDTH = width;
    glob::HEIGHT = height;
    glob::proj = Eigen::perspective(50.0f, (float)width / (float)height, 0.1f, 1000.0f);
}

/**
 * @brief mouse_button: Mouse button handling function
 * @param button: Button index
 * @param action: Button action
 */
void GLFWCALL mouse_button(int button, int action) {
    if (!TwEventMouseButtonGLFW(button, action)) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            int x_i, y_i;
            glfwGetMousePos(&x_i, &y_i);
            vec2 p = transform_screen_coords(x_i, y_i);
            trackball.begin_drag(p.x(), p.y());
        }
    }
}

/**
 * @brief mouse_pos: Mouse position callbacl handling function
 * @param x
 * @param y
 */
void GLFWCALL mouse_pos(int x, int y) {
    if (!TwEventMousePosGLFW(x, y)) {
        if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            vec2 p = transform_screen_coords(x, y);
            trackball.drag(p(0), p(1));
        }
    }
}

/**
 * @brief Mouse_Wheel Mouse Wheel callbacl handling function
 * @param pos
 */
void GLFWCALL Mouse_Wheel(int pos){
    static float zoom_pos = 0.0f;
    campos = campos + ((float) pos - zoom_pos) * camdir;
    zoom_pos = pos;
}

/**
 * @brief keyboard: keyboard function handling
 * @param key: key type
 * @param action: key action
 */
void GLFWCALL keyboard(int key, int action){
    if(action != GLFW_RELEASE) {
        switch(key){
        case '`':
            /* press '`' to terminate the program */
            clean_bar();
            glfwTerminate();
            std::cout << "---User Terminates---" << std::endl;
            break;
        case 'W':
            time_prev = glfwGetTime();
            break;
        case 'S':
            time_prev = glfwGetTime();
            break;
        default:
            break;
        }
    }
}

mat3 rodrigue_matrix(const vec3& n, float cosa){
    mat3 rom = mat3::Identity();
    mat3 N;
    N << 0.00f, +n(2), -n(1),
         -n(2), 0.00f, +n(0),
         +n(1), -n(0), 0.00f;
    rom *= cosa;
    rom += (1 - cosa) * n * n.transpose();
    rom += sqrt(1 - cosa * cosa) * N;
    return rom;
}

void move_camera() {
    if (glfwGetKey(GLFW_KEY_UP   ) == GLFW_PRESS) { origin(1) += (float)0.001; }
    if (glfwGetKey(GLFW_KEY_DOWN ) == GLFW_PRESS) { origin(1) -= (float)0.001; }
    if (glfwGetKey(GLFW_KEY_LEFT ) == GLFW_PRESS) { origin(0) -= (float)0.001; }
    if (glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS) { origin(0) += (float)0.001; }
    if (glfwGetKey((int)'W') == GLFW_PRESS) {
        float dt = glfwGetTime() - time_prev;
        time_prev += dt;
        if (move_velo < -0.0) { move_velo += 20.0 * dt; }
        else { move_velo += 6.0 * dt; }
        move_velo = move_velo >  max_velo ?  max_velo : move_velo;
        campos += camdir * move_velo * dt;
    }
    if (glfwGetKey((int)'S') == GLFW_PRESS) {
        float dt = glfwGetTime() - time_prev;
        time_prev += dt;
        if (move_velo > 0.0) { move_velo -= 20.0 * dt; }
        else { move_velo -= 6.0 * dt; }
        move_velo = move_velo < -max_velo ? -max_velo : move_velo;
        campos += camdir * move_velo * dt;
    }
    if (glfwGetKey((int)'W') == GLFW_RELEASE && glfwGetKey((int)'S') == GLFW_RELEASE) {
        if (move_velo > 0.5) {
            float dt = glfwGetTime() - time_prev;
            time_prev += dt;
            move_velo -= 20.0 * dt;
            campos += camdir * move_velo * dt;
        } else if (move_velo < -0.5) {
            float dt = glfwGetTime() - time_prev;
            time_prev += dt;
            move_velo += 20.0 * dt;
            campos += camdir * move_velo * dt;
        } else { move_velo = 0.0; }
    }
    if (glfwGetKey((int)'A') == GLFW_PRESS) { camdir = rodrigue_matrix(-camupv, cos(0.02)) * camdir; }
    if (glfwGetKey((int)'D') == GLFW_PRESS) { camdir = rodrigue_matrix( camupv, cos(0.02)) * camdir; }
    if (glfwGetKey((int)'Q') == GLFW_PRESS) {
        vec3 right =  camdir.cross(camupv);
        mat3 rotat = rodrigue_matrix(right, cos(0.01));
        camdir = rotat * camdir;
        camupv = rotat * camupv;
    }
    if (glfwGetKey((int)'E') == GLFW_PRESS) {
        vec3 right = -camdir.cross(camupv);
        mat3 rotat = rodrigue_matrix(right, cos(0.01));
        camdir = rotat * camdir;
        camupv = rotat * camupv;
    }
}

/**
 * @brief get_2dtga_pixel: obtain '.tga' 2D texture data
 * @param name: filename
 * @param width: image width
 * @param height: image height
 * @param format: image color format
 * @param type:
 * @return image data
 */
GLvoid* get_2dtga_pixel(const char* name, int width, int height, GLenum format, GLenum type){
    GLuint tmp;
    GLvoid* img = new float[width * height * 3];
    glGenTextures(1, &tmp);
    glBindTexture(GL_TEXTURE_2D, tmp);
    glfwLoadTexture2D(name, 0);
    glGetTexImage(GL_TEXTURE_2D, 0, format, type, img);
    return img;
}
#endif

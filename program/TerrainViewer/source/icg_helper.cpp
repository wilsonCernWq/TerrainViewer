#include "icg_helper.h"

/**
 * @brief LookAt Setup view matrix
 * @param eye Camera position
 * @param center Looking center
 * @param up Up vector
 * @return
 */
mat4 LookAt(vec3 eye, vec3 view, vec3 up)
{
    vec3 z_cam = view.normalized();
    vec3 x_cam = up.cross(z_cam).normalized();
    vec3 y_cam = z_cam.cross(x_cam);
    mat3 R = mat3::Zero();
    R.col(0) = x_cam;
    R.col(1) = y_cam;
    R.col(2) = z_cam;
    mat4 L = mat4::Zero();
    L.block(0, 0, 3, 3) =  R.transpose();
    L(3, 3) = 1.0f;
    L.block(0, 3, 3, 1) = -R.transpose() * (eye);
    return L;
}

/**
 * @brief OrthographicProjection: Calculate orthographic projection matrix
 * @param left: left boundary
 * @param right: right boundary
 * @param bottom: bottom boundary
 * @param top: top boundary
 * @param n: n boundary
 * @param f: f boundary
 * @return 4X4 orthographic projection matrix
 */
mat4 OrthographicProjection(float l, float r, float b, float t, float n, float f)
{
    mat4 ortho = mat4::Zero();
    ortho(0, 0) =  2.0f / (r - l);
    ortho(1, 1) =  2.0f / (t - b);
    ortho(2, 2) = -2.0f / (f - n);
    ortho(3, 3) =  1.0f;
    ortho(1, 3) = -(r + l) / (r - l);
    ortho(2, 3) = -(t + b) / (t - b);
    ortho(2, 3) = -(f + n) / (f - n);
    return ortho;
}

/**
 * @brief PerspectiveProjection: Calculate perspective projection matrix
 * @param left: left boundary
 * @param right: right boundary
 * @param bottom: bottom boundary
 * @param top: top boundary
 * @param n: n boundary
 * @param f: f boundary
 * @return 4X4 perspective projection matrix
 */
mat4 PerspectiveProjection(float l, float r, float b, float t, float n, float f)
{
    mat4 frust = mat4::Zero();
    frust(0, 0) =  2.0f * n / (r - l);
    frust(1, 1) =  2.0f * n / (t - b);
    frust(2, 3) = -2.0f * f * n / (f - n);
    frust(2, 2) = -(f + n) / (f - n);
    frust(3, 2) = -1.0f;
    frust(0, 2) =  (r + l) / (r - l);
    frust(1, 2) =  (t + b) / (t - b);
    return frust;
}

/**
 * @brief rodrigue_matrix Calculate Rotational matrix by rotational axis and angle
 * @param n: rotational axis
 * @param c: cos value of rotational angle
 * @return
 */
mat3 rodrigue_matrix(const vec3& n, float c)
{
    mat3 rom = mat3::Identity();
    mat3 N;
    N << 0.00f, +n(2), -n(1),
         -n(2), 0.00f, +n(0),
         +n(1), -n(0), 0.00f;
    rom *= c;
    rom += (1.0f - c) * n * n.transpose();
    rom += sqrt(1.0f - c * c) * N;
    return rom;
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
GLvoid* get_2dtga_pixel(const char* name, int width, int height, GLenum format, GLenum type)
{
    GLuint tmp;
    GLvoid* img = new float[width * height * 3];
    glGenTextures(1, &tmp);
    glBindTexture(GL_TEXTURE_2D, tmp);
    glfwLoadTexture2D(name, 0);
    glGetTexImage(GL_TEXTURE_2D, 0, format, type, img);
    return img;
}

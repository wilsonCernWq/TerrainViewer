#ifndef _ICG_HELPER_H_
#define _ICG_HELPER_H_

/** @brief Definition of helper functions for ICG project */

#include "icg_common.h"

typedef Eigen::Transform<float,3,Eigen::Affine> Transform;
typedef unsigned char Pixel;
struct MultiFractal {
    float lambda;
    float lacunarity;
    float octaves;
    float gain;
    float offset;
    float center;
};

/**
 * @brief transform_screen_coords: Transform screen position into world coordinates
 * @param x: x coordinate
 * @param y: y coordinate
 * @return Corresponding world coordinates
 */
inline void icgScreen(int x, int y, float width, float height, vec2& p)
{
    p = vec2(2.0f * (float)x / width - 1.0f, 1.0f - 2.0f * (float)y / height);
}

/**
 * @brief LookAt Setup view matrix
 * @param eye Camera position
 * @param center Looking center
 * @param up Up vector
 * @return
 */
mat4 LookAt(vec3 eye, vec3 view, vec3 up);

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
mat4 OrthographicProjection(float l, float r, float b, float t, float n, float f);

/**
 * @brief OrthographicProjection:Overload of original function with an alternative input format
 * @param top: top boundary
 * @param right: right boundary
 * @param near: near boundary
 * @param far: far boundary
 * @return
 */
inline mat4 OrthographicProjection(float t, float r, float n, float f)
{
    assert(t > 0);
    assert(r > 0);
    assert(n < f);
    return OrthographicProjection(-r, r, -t, t, n, f);
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
mat4 PerspectiveProjection(float l, float r, float b, float t, float n, float f);

/**
 * @brief PerspectiveProjection: Overload of original function with an alternative input format
 * @param fovy: arctan(top/near)
 * @param aspect: right / top
 * @param near: near boundary
 * @param far: far boundary
 * @return
 */
inline mat4 PerspectiveProjection(float r, float a, float n, float f)
{
    assert(a > 0);
    assert(n > 0);
    assert(n < f);
    float t = n * std::tan(M_PI * r / 180.0);
    return PerspectiveProjection(-t * a, t * a, -t, t, n, f);
}

/**
 * @brief rodrigue_matrix Calculate Rotational matrix by rotational axis and angle
 * @param n: rotational axis
 * @param c: cos value of rotational angle
 * @return
 */
mat3 rodrigue_matrix(const vec3& n, float c);

/**
 * @brief get_2dtga_pixel: obtain '.tga' 2D texture data
 * @param name: filename
 * @param width: image width
 * @param height: image height
 * @param format: image color format
 * @param type:
 * @return image data
 */
GLvoid* get_2dtga_pixel(const char* name, int width, int height, GLenum format, GLenum type);

#endif // _ICG_HELPER_H_

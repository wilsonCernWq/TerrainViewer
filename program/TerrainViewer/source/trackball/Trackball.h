#ifndef TRACKBALL_H
#define TRACKBALL_H
#include "icg_common.h"
#include "../icg_helper.h"

/**
 * @brief The Trackball class: Trackball function implementation
 */
class Trackball {
public:
    /**
     * @brief Trackball: Constructor
     * @param r: trackball radius
     */
    Trackball(float r = 1.0f) : _radius(r), _mat(mat4::Identity()) {}

    /**
     * @brief begin_drag: initialize drag
     * @param x: previous x position
     * @param y: previous y position
     */
    void begin_drag(float x, float y);

    /**
     * @brief drag: execute drag
     * @param x: current x position
     * @param y: current y position
     */
    void drag(float x, float y);

    /**
     * @brief matrix: trackball matrix accessor
     * @return current trackball matrix
     */
    mat4& matrix() { return _mat; }

private:
    /**
     * @brief _project_onto_surface: project (x,y) mouse pos on surface
     * @param x: X position
     * @param y: Y position
     * @return projected position
     */
    vec3 _project_onto_surface(const float x, const float y) const;

private:
    float _radius;  ///< trackball radius
    vec3  _prev_pos; ///< previous mouse position
    vec3  _pos;      ///< current mouse position
    mat4  _prev_mat; ///< previous trackball matrix
    mat4  _mat;      ///< current trackball matrix
};

#endif

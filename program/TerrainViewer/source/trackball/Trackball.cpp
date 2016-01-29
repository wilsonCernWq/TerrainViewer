#include "Trackball.h"

/**
     * @brief begin_drag: initialize drag
     * @param x: previous x position
     * @param y: previous y position
     */
void Trackball::begin_drag(float x, float y) {
    _prev_pos = _project_onto_surface(x, y);
    _prev_mat = _mat;
}

/**
     * @brief drag: execute drag
     * @param x: current x position
     * @param y: current y position
     */
void Trackball::drag(float x, float y) {
    _pos = _project_onto_surface(x, y);
    mat4 rot = mat4::Identity();
    vec3 dir = -_prev_pos.cross(_pos).normalized();
    rot.block(0,0,3,3)
            = rodrigue_matrix(dir,_prev_pos.dot(_pos)/_prev_pos.norm()/_pos.norm());
    _mat = rot * _prev_mat;
}

/**
     * @brief _project_onto_surface: project (x,y) mouse pos on surface
     * @param x: X position
     * @param y: Y position
     * @return projected position
     */
vec3 Trackball::_project_onto_surface(const float x, const float y) const {
    float r = x * x + y * y;
    float R = 0.5 * _radius * _radius;
    float z = r > R ? R / sqrt(r) : sqrt(2.0 * R - r);
    return vec3(x,y,z);
}


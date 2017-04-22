#ifndef TRACKBALL_H
#define TRACKBALL_H
#include "icg_common.h"

/**
 * @brief The Trackball class: Trackball function implementation
 */
class Trackball {
public:
    /**
     * @brief Trackball: Constructor
     * @param r: trackball radius
     */
    Trackball(float r = 1.0f) : _radius(r), _matrix(mat4::Identity()) {}

    /**
     * @brief begin_drag: initialize drag
     * @param x: previous x position
     * @param y: previous y position
     */
    void begin_drag(float x, float y) {
        _anch_pos = _project_onto_surface(vec3(x, y, 0.0f));
        _tmp_matrix = _matrix;
    }

    /**
     * @brief drag: execute drag
     * @param x: current x position
     * @param y: current y position
     */
    void drag(float x, float y) {
        mat4 rot = mat4::Identity();
        vec3 cur_pos = _project_onto_surface(vec3(x, y, 0.0f));
        vec3 axis = -_anch_pos.cross(cur_pos).normalized();
        float cosa = _anch_pos.dot(cur_pos) / (_anch_pos.norm() * cur_pos.norm());
        rot.block(0, 0, 3, 3) = _rodrigue_matrix(axis, cosa);
        _matrix = rot * _tmp_matrix;
    }

    /**
     * @brief matrix: trackball matrix accessor
     * @return current trackball matrix
     */
    mat4& matrix() { return _matrix; }

private:
    /**
     * @brief _rodrigue_matrix: compute Rodrigue matrix
     * @param n: rotational axis
     * @param cosa: cosine value of rotational angle
     * @return Rodrigue matirx
     */
    mat3 _rodrigue_matrix(const vec3& n, float cosa){
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

    /**
     * @brief _project_onto_surface: project mouse position onto 2D surface
     * @param p: mouse position
     * @return projected position
     */
    vec3 _project_onto_surface(const vec3& p) const {
        vec3 var(p);
        //-> Conpute norm
        float r1 = p(0) * p(0) + p(1) * p(1);
        float r2 = _radius * _radius;
        if (2 * r1 > r2) {
            var(2) = r2 / (2 * sqrt(r1));
        } else {
            var(2) = sqrt(r2 - r1);
        }
        return var;
    }

private:
    float _radius;  ///< trackball radius
    /// @todo trackball member variable
    vec3 _anch_pos;  ///< previous mouse position
    mat4 _tmp_matrix;  ///< previous trackball matrix
    mat4 _matrix;  ///< current trackball matrix
};

#endif

//!
//! This file defines the trackball class, which is independent to the project
//! itself.
//!
#pragma once
#ifndef _TRACKBALL_H_
#define _TRACKBALL_H_

#include "../common/icg_common.h"
#include "ball.h"

namespace icg {
    //!
    //! following the implementation of
    //!   http://image.diku.dk/research/trackballs/index.html
    //!
    class Trackball : public Ball {
    private:
        Ball* world = nullptr;
        float radius = 1.0f;
        bool flag_inverse = false;
        cy::Matrix4f matrix = cy::Matrix4f::MatrixIdentity();
        cy::Matrix4f matrix_prev = cy::Matrix4f::MatrixIdentity();
        cy::Point3f position = cy::Point3f(0.0f, 0.0f, 0.0f);
        cy::Point3f position_prev = cy::Point3f(0.0f, 0.0f, 0.0f);
    public:
        //! Constructor
        //! \param world_camera: the reference camera used to adjust screen rotation
        Trackball(Ball* world_camera) { world = world_camera; }

        //! SetRadius
        //! \param r set trackball radius
        void SetRadius(const float r) { radius = r; }

        //! SetModeInverse
        //! \param flag
        void SetModeInverse(bool flag) { flag_inverse = flag; }

        //! BeginDrag
        //! \param x screen coordinate
        //! \param y screen coordinate
        void BeginDrag(float x, float y)
        {
          position_prev = proj2surf(x, y);
          matrix_prev = matrix;
        }

        void Drag(float x, float y)
        {
          // get direction
          position = proj2surf(x, y);
          cy::Point3f dir = (position_prev.Cross(position)).GetNormalized();
          // modify
          if (world != nullptr) {
            auto inverse = world->Matrix().GetInverse();
            dir = cy::Point3f(inverse * cy::Point4f(dir, 0.0f));
          }
          dir = flag_inverse ? -dir : dir;
          // compute rotation angle
          float product_dot = position_prev.Dot(position);
          float product_len = position_prev.Length() * position.Length();
          float angle = std::min(1.0f, product_dot / product_len);
          cy::Matrix4f rotation;
          if (angle > 0.9999) {
            // prevent position_prev == position
            return;
          }
          else { // compute rotation
            rotation.SetRotation(dir, static_cast<float>(acos(angle)));
          }
          matrix = rotation * matrix_prev;
        }

        cy::Matrix4f Matrix()
        {
          return matrix;
        }

        void Reset() { matrix = cy::Matrix4f::MatrixIdentity(); }

    private:
        //! proj2surf: project (x,y) mouse pos on surface
        //! @param x: X position
        //! @param y: Y position
        //! @return projected position
        cy::Point3f proj2surf(const float x, const float y) const
        {
          float r = x * x + y * y;
          float R = radius * radius;
          float z = r > R / 2.0f ? R / 2.0f / static_cast<float>(sqrt(r)) : static_cast<float>(sqrt(R - r));
          return cy::Point3f(x, y, z);
        }

    };
};

#endif//_TRACKBALL_H_

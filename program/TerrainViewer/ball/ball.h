//!
//! This file defines the trackball class, which is independent to the project itself.
//!
#pragma once
#ifndef _BALL_H_
#define _BALL_H_

#include "../common/icg_common.h"

namespace icg 
{
	//!
	//! abstract class for ball rotation
	//!
	class Ball {
	private:
		bool disable_zoom = false;
		float zoomRatio[3] = { 0.0f, 0.0f, 0.0f };
	public:

		//! constractors
		Ball() {}

		//! destractors
		virtual ~Ball() {}

		//! BeginDrag: initialize drag
		//! @param x: previous x position
		//! @param y: previous y position
		virtual void BeginDrag(float x, float y) = 0;

		//! BeginZoom: initialize zoom
		//! @param x: previous x position
		//! @param y: previous y position
		virtual void BeginZoom(float x, float y)
		{
			zoomRatio[1] = zoomRatio[0];
			zoomRatio[2] = y;
		}

		//! Drag: execute Drag
		//! @param x: current x position
		//! @param y: current y position
		virtual void Drag(float x, float y) = 0;

		//! Zoom: execute Zoom
		//! @param x: current x position
		//! @param y: current y position
		virtual void Zoom(float x, float y) {
			zoomRatio[0] = y - zoomRatio[2] + zoomRatio[1];
		}

		//! Matrix: ball matrix accessor
		//! @return current ball matrix
		virtual cy::Matrix4f Matrix() = 0;

		//! ZoomRatio: zooming distance
		//! @return current zooming ratio
		virtual float ZoomRatio() { return zoomRatio[0]; }

		//! Reset: reset the matrix to default mode
		virtual void Reset() = 0;

		//! SetZoomMode: set the flag for zooming
		virtual void SetModeZoom(bool flag) { disable_zoom = flag; }
	};

};

#endif//_BALL_H_

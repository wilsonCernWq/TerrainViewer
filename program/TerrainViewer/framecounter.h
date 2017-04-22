#pragma once

#ifndef _FRAME_COUNTER_H_
#define _FRAME_COUNTER_H_

#include "configure.h"

namespace icg {

	class FrameCounter {
	private:
		const int reset_frequency = 10;
		cy::Timer timer;
		int offset = 0;
		int frame_count = 0;
		float fps = 0.0f;
	public:

		void Count() {
			frame_count++;
			if (frame_count == 0) {
				timer.Start();
			}
			else if (frame_count - offset >= reset_frequency) {
				fps = (frame_count - offset) / timer.Stop();
				offset = frame_count;
				timer.Start();
			}
		}

		float FPS() { return fps; }

	};

};

#endif//_FRAME_COUNTER_H_
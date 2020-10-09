#pragma once

#include <chrono>
#include "Basics.hpp"

using TimePoint = std::chrono::steady_clock::time_point;

using Duration = std::chrono::duration<Double, std::ratio< 1 / 1 >>;

struct Time {
	Bool paused = false;
	TimePoint lastTime;
	TimePoint currentTime;
	Duration deltaDuration = std::chrono::seconds::zero();
	Duration totalDuration = std::chrono::seconds::zero();
	Float delta = 0.0f;
	Float total = 0.0f;

	void reset();
	void update();
};
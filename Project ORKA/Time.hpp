#pragma once

#include <chrono>
#include "Basics.hpp"

using Clock = std::chrono::system_clock;
using Milliseconds = std::chrono::milliseconds;
using TimePoint = Clock::time_point;
using Duration = std::chrono::duration<Double, std::ratio< 1 / 1 >>;
using FHours = std::chrono::duration<float, std::chrono::hours::period>;
TimePoint now();

struct Time {
	Bool paused = false;
	TimePoint lastTime;
	TimePoint currentTime;
	Duration deltaDuration = Duration(0);
	Duration totalDuration = Duration(0);
	Float delta = 0.0f;
	Float total = 0.0f;

	void reset();
	void pause();
	void update();
	void unpause();
};

void sleep();
void sleep(UInt millseconds);
void sleepUntil(TimePoint& t);
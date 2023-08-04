#pragma once

#include <chrono>
#include "Basics.hpp"

// we just override some names to make them look nicer
using Clock = std::chrono::system_clock;
using Milliseconds = std::chrono::milliseconds;
using TimePoint = Clock::time_point;
using Duration = std::chrono::duration<Double, std::ratio<1 / 1>>;
using FHours = std::chrono::duration<float, std::chrono::hours::period>;

// a simple function that returns the current time point
TimePoint now();

// a timer that can keep track of certain things, like delta from one update to the other or total execution time
struct Time
{
	TimePoint lastTime;
	TimePoint currentTime;
	Bool      paused        = false;
	Float     delta         = 0.0f;
	Float     total         = 0.0f;
	Duration  deltaDuration = Duration(0);
	Duration  totalDuration = Duration(0);

	void reset();
	void pause();
	void update();
	void unpause();
};

// a simple timer that can measure the time between execution of start and stop
struct Timer
{
	TimePoint stopTP;
	TimePoint startTP;
	Float     delta         = 0.0f;
	Duration  deltaDuration = Duration(0);

	void stop();
	void start();
};

// puts threads to sleep for a certain amount of time
void sleep();
void sleep(UInt milliseconds);
void sleepUntil(const TimePoint& t);

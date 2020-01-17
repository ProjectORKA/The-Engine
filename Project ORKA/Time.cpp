
#include "Time.hpp"

float Time::delta()
{
	return deltaDuration.count();
}

float Time::total()
{
	return totalDuration.count();
}

void Time::resetTime()
{
	currentTime = lastTime = std::chrono::steady_clock::now();
	deltaDuration = std::chrono::seconds::zero();
	totalDuration = std::chrono::seconds::zero();
}

void Time::processTime()
{
	lastTime = currentTime;
	currentTime = std::chrono::steady_clock::now();

	if (!paused) {
		deltaDuration = currentTime - lastTime;
		totalDuration += deltaDuration;
	}
	else {
		deltaDuration = std::chrono::seconds::zero();
	}
}
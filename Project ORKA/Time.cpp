
#include "Time.hpp"

void Time::reset(){
	currentTime = lastTime = std::chrono::steady_clock::now();
	deltaDuration = std::chrono::seconds::zero();
	totalDuration = std::chrono::seconds::zero();
	delta = 0.0f;
	total = 0.0f;
}

void Time::update() {
	lastTime = currentTime;
	currentTime = std::chrono::steady_clock::now();

	if (!paused) {
		deltaDuration = currentTime - lastTime;
		totalDuration += deltaDuration;
		delta = deltaDuration.count();
		total = totalDuration.count();
	}
	else {
		deltaDuration = std::chrono::seconds::zero();
	}
}
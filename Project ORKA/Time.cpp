#include "Time.hpp"
#include "Debug.hpp"

TimePoint now() { return Clock::now(); }

void Time::reset() {
	currentTime = now();
	lastTime = now();
	deltaDuration = Duration(0);
	totalDuration = Duration(0);
	delta = 0.0f;
	total = 0.0f;
}

void Time::pause() { paused = true; }

void Time::update() {
	lastTime = currentTime;
	currentTime = now();

	if (!paused) {
		deltaDuration = currentTime - lastTime;
		totalDuration += deltaDuration;
		delta = deltaDuration.count();
		total = totalDuration.count();
	}
	else { deltaDuration = std::chrono::seconds::zero(); }
}

void Time::unpause() { paused = false; }

void Timer::start() { startTP = now(); }

void Timer::stop() {
	stopTP = now();
	deltaDuration = stopTP - startTP;
	delta = deltaDuration.count();
	logDebug(String("Process took (").append(std::to_string(delta)).append(") seconds."));
}

void sleep() { std::this_thread::sleep_for(std::chrono::nanoseconds(1)); }
void sleep(const UInt millseconds) { std::this_thread::sleep_for(std::chrono::milliseconds(millseconds)); }
void sleepUntil(const TimePoint& t) { std::this_thread::sleep_until(t); }

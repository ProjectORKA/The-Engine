
#include "Time.hpp"

TimePoint now() {
	return Clock::now();
}

void Time::reset(){
	currentTime = lastTime = now();
	deltaDuration = Duration(0);
	totalDuration = Duration(0);//std::chrono::seconds::zero();
	delta = 0.0f;
	total = 0.0f;
}
void Time::pause()
{
	paused = true;
}
void Time::update() {
	lastTime = currentTime;
	currentTime = now();

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
void Time::unpause()
{
	paused = false;
}

void sleep()
{
	std::this_thread::sleep_for(std::chrono::nanoseconds(1));
}
void sleep(UInt millseconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(millseconds));
}
void sleepUntil(TimePoint& t)
{
	std::this_thread::sleep_until(t);
}
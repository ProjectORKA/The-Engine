#include "Time.hpp"
#include "Debug.hpp"

void sleep()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

TimePoint now()
{
	return Clock::now();
}

void Time::reset()
{
	currentTime   = now();
	lastTime      = now();
	deltaDuration = Duration(0);
	totalDuration = Duration(0);
	delta         = 0.0f;
	total         = 0.0f;
}

void Time::pause()
{
	paused = true;
}

void Timer::stop()
{
	stopTP        = now();
	deltaDuration = stopTP - startTP;
	delta         = static_cast<Float>(deltaDuration.count());
	logDebug("Process took (" + toString(delta) + ") seconds.");
}

void Time::update()
{
	lastTime    = currentTime;
	currentTime = now();

	if(!paused)
	{
		deltaDuration = currentTime - lastTime;
		totalDuration += deltaDuration;
		delta = static_cast<Float>(deltaDuration.count());
		total = static_cast<Float>(totalDuration.count());
	}
	else deltaDuration = std::chrono::seconds::zero();
}

void Timer::start()
{
	startTP = now();
}

void Time::unpause()
{
	paused = false;
}

void sleep(const UInt milliseconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void sleepUntil(const TimePoint& t)
{
	std::this_thread::sleep_until(t);
}

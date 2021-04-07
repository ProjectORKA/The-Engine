#include "Timer.hpp"
#include "Debug.hpp"

void Timer::start()
{
	startTP = std::chrono::steady_clock::now();
}

float Timer::stop()
{
	stopTP = std::chrono::steady_clock::now();
	auto duration = stopTP - startTP;
	logDebug(String("Process took (").append(std::to_string(duration.count())).append(") nanoseconds."));
	return 0;// duration.count();
}

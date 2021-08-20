#include "Timer.hpp"
#include "Debug.hpp"

void Timer::start()
{
	startTP = now();  
}

float Timer::stop()
{
	stopTP = now();
	auto duration = stopTP - startTP;
	logDebug(String("Process took (").append(std::to_string(duration.count())).append(") nanoseconds."));
	return 0;// duration.count();
}

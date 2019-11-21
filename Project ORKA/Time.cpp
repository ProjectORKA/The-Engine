
#include "Program.hpp"

void resetTime(Time & time) {
	time.currentTime = time.lastTime = std::chrono::steady_clock::now();
	time.delta = std::chrono::seconds::zero();
	time.total = std::chrono::seconds::zero();
}

void updateTime(Time & time) {
	time.lastTime = time.currentTime;
	time.currentTime = std::chrono::steady_clock::now();

	if (!time.paused) {
		time.delta = time.currentTime - time.lastTime;
		time.total += time.delta;
	}
	else {
		time.delta = std::chrono::seconds::zero();
	}
}


double Time::getDelta() {
	return delta.count();
}

double Time::getTotal() {
	return total.count();
}

Time::Time() {
	resetTime(*this);
	debugPrint("|--Time was created!");
}

Time::~Time()
{
	debugPrint("|--Time was destroyed!");
}
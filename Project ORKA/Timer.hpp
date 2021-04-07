
#pragma once

#include "Time.hpp"

struct Timer {
	TimePoint startTP;
	TimePoint stopTP;

	void start();
	float stop();

};
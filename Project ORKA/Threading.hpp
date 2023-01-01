
#pragma once

#include "Basics.hpp"
#include "Debug.hpp"
#include <thread>

struct Thread {
	// its just here to hide some ugly stuff away
	// threads usually keep track of one or more objects
	// those objects should have a "keeprunning" value if you plan on using while loops
	// other threads will execute and end by themselves

	std::thread thread;

	template<class Function, class ...Args>
	void start(Function&& f, Args && ...args)
	{
		thread = std::thread(f, std::ref(args)...);
	}
	void stop();
};


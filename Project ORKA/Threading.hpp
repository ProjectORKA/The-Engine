
#pragma once

#include "Basics.hpp"
#include "Debug.hpp"
#include <thread>

struct Thread {
	// its just here to hide some ugly stuff away
	// what some threads do, is that they store a reference to this object
	// they can then look at the "keepThreadRunning" value and exit any loop they are in
	// this means the stop() function can do a bit more than just join

	std::thread thread;
	Bool keepThreadRunning = false;

	template<class Function, class ...Args>
	void start(Function&& f, Args && ...args)
	{
		keepThreadRunning = true;
		thread = std::thread(f, std::ref(args)...);
	}
	void stop()
	{
		if (keepThreadRunning) {
			keepThreadRunning = false;
		}
		thread.join();
	}
};
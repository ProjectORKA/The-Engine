
#pragma once

#include "Basics.hpp"
#include "Debug.hpp"
#include <thread>

struct Thread {
	std::thread thread;
	Bool keepThreadRunning = false;

	template<class Function, class ...Args>
	void start(Function&& f, Args && ...args)
	{
		keepThreadRunning = true;
		thread = std::thread(f, std::ref(args)...);
	};

	void stop();
};
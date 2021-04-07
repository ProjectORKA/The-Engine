#pragma once

#include "Basics.hpp"

struct JobSystem {
	Vector<std::thread> workerThreads;
	
	void create(UShort numberOfThreads);
};

extern JobSystem jobSystem;
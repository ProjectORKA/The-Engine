#pragma once

#include "Basics.hpp"

class JobSystem;

void workerThread(JobSystem& jobSystem);

class JobSystem
{
public:
	// start job system
	void start();
	void start(UInt numThreads);
	// stop job system
	void waitStop();
	void forceStop();
	// add work
	template <typename Func, typename... Args> void enqueue(Func&& function, Args&&... args);
	// extra stuff
	[[nodiscard]] Bool isRunning() const;
	Function<void()>   next();
private:
	Mutex                  mutex;
	Bool                   running = true;
	Vector<Thread>         workers;
	List<Function<void()>> jobQueue;
};

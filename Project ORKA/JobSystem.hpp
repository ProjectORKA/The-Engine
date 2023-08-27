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

	void wait() const;

	// add work
	template <typename Func, typename... Args> void enqueue(Func&& function, Args&&... args)
	{
		LockGuard                   lock(mutex);
		const std::function<void()> job = std::bind(std::forward<Func>(function), std::forward<Args>(args)...);
		jobQueue.push_back(job);
	}

	// extra stuff
	Function<void()>   next();
	[[nodiscard]] Bool isRunning() const;

private:
	Mutex                  mutex;
	Bool                   running = true;
	Vector<Thread>         workers;
	List<Function<void()>> jobQueue;
};

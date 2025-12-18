#pragma once

#include "Basics.hpp"
#include "Debug.hpp"
#include "JobQueue.hpp"

// CPUs can not only do things one after the other (in serial), but also multiple things at the same time (parallel)
// the thing that allows us to run things in parallel are threads
// one way to manage work across those threads is a "job system"
// a jobsystem manages "work" done by "workers" such that every worker is always busy
// this means there is a list of work that every worker checks continuously

// [TODO] try lock free / wait free solutions

struct JobSystem;

extern JobSystem jobSystem;

struct JobSystem
{
private:
	JobSystemQueue jobQueue;
	Vector<Thread> workers;
	Int            threadID       = 1;
	Bool           running        = false; // if disabled prevents workers from pulling new work
	Bool           canEnqueueNext = false; // if disabled prevents work from creating subsequent work
	Bool           canEnqueueNew  = false; // if disabled prevents new work being added

	Job  next(Int threadID);
	void startWithThreads(UInt numThreads);

public:
	void stop();
	void start();
	void letStopAndJoin();
	void waitStopAndJoin();
	void forceStopAndJoin();

	[[nodiscard]] ULL workCount() const;
	[[nodiscard]] ULL numThreads() const;

	static void workerThread(Int threadID);

	template <typename Func, typename... Args>
	void enqueue(Func&& function, Args&&... args)
	{
		if (debugJobSystemIsEnabled) logDebug("Add new job to queue!");
		if (canEnqueueNew) jobQueue.enqueue(std::bind(std::forward<Func>(function), std::forward<Args>(args)...));
		else if (debugJobSystemIsEnabled) logWarning("Could not enqueue new job!");
	}

	template <typename Func, typename... Args>
	void enqueueNext(Func&& function, Args&&... args)
	{
		if (debugJobSystemIsEnabled) logDebug("Add subsequent job!");
		if (canEnqueueNext) jobQueue.enqueue(std::bind(std::forward<Func>(function), std::forward<Args>(args)...));
		else if (debugJobSystemIsEnabled) logWarning("Could not enqueue next job!");
	}
};

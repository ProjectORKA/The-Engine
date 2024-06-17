#include "JobSystem.hpp"

JobSystem jobSystem;

Job JobSystem::next(const Int threadID)
{
	// this function pulls the next job from the stack
	return jobQueue.dequeue(threadID);
}

void JobSystem::start()
{
	// this function just starts the job system with the most optimal number of threads

	const UInt numberOfThreads = Thread::hardware_concurrency();
	startWithThreads(numberOfThreads);
}

void JobSystem::letStopAndJoin()
{
	// waits for all jobs to be finished
	// WARNING: locks, if infinite jobs exist

	if (debugJobSystemIsEnabled) logDebug("Let Jobs finish.");
	while (!jobQueue.empty()) {}
	running = false;
	jobQueue.clear();
	for (Thread& worker : workers) worker.join();
}

void JobSystem::waitStopAndJoin()
{
	// prevents new jobs and waits for all subsequent jobs to be finished

	if (debugJobSystemIsEnabled) logDebug("Finish current jobs!");
	canEnqueueNew = false;
	while (!jobQueue.empty()) {}
	running = false;
	jobQueue.clear();
	for (Thread& worker : workers) worker.join();
}

void JobSystem::forceStopAndJoin()
{
	// makes the system halt as soon as possible
	// still waits for all current work to be done
	// if this takes too long, you might have to split your work into smaller tasks

	if (debugJobSystemIsEnabled) logDebug("Force jobs to quit!");
	canEnqueueNew  = false;
	canEnqueueNext = false;
	running        = false;
	jobQueue.clear();
	for (Thread& worker : workers) worker.join();
}

void JobSystem::workerThread(const Int threadID)
{
	// this function is the logic for each worker
	// basically they just always check for new stuff to do

	if (debugJobSystemIsEnabled) logDebug("Worker thread running with id (" + toString(threadID) + ")");

	while (jobSystem.running)
	{
		Job job = jobSystem.next(threadID);
		job();
	}

	if (debugJobSystemIsEnabled) logDebug("Worker thread (" + toString(threadID) + ") exited.");
}

ULL JobSystem::workCount() const
{
	return jobQueue.size();
}

ULL JobSystem::numThreads() const
{
	return workers.size();
}

void JobSystem::stop()
{
	canEnqueueNew  = false;
	canEnqueueNext = false;
	while(!jobQueue.empty()){}
}

void JobSystem::startWithThreads(const UInt numThreads)
{
	// starts the job system with a specified number of workers and enables adding work to it 

	running        = true;
	canEnqueueNew  = true;
	canEnqueueNext = true;

	if (debugJobSystemIsEnabled) logDebug("Job system running with " + toString(numThreads) + " threads");

	for (UInt i = 0; i < numThreads; ++i) workers.emplace_back(workerThread, threadID++);
}

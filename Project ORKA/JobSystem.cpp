#include "JobSystem.hpp"
#include "Debug.hpp"
#include "Random.hpp"

void JobSystem::start()
{
	const UInt numberOfThreads = Thread::hardware_concurrency();
	start(numberOfThreads);
}

void JobSystem::waitStop()
{
	while(!jobQueue.empty()) {}
	forceStop();
}

void JobSystem::forceStop()
{
	// LockGuard lock(mutex);
	running = false;
	jobQueue.clear();
	for(Thread& worker : workers) worker.join();
}

Bool JobSystem::isRunning() const
{
	return running;
}

Function<void()> JobSystem::next()
{
	LockGuard lock(mutex);
	if(jobQueue.empty()) return {};
	auto result = std::move(jobQueue.front());
	jobQueue.pop_front();
	return result;
}

void workerThread(JobSystem& jobSystem)
{
	while(jobSystem.isRunning()) if(auto job = jobSystem.next()) job();
}

void JobSystem::start(const UInt numThreads)
{
	running = true;
	logDebug("Job system running with " + toString(numThreads) + " threads");

	for(UInt i = 0; i < numThreads; ++i) workers.emplace_back(workerThread, std::ref(*this));
}

template <typename Func, typename... Args> void JobSystem::enqueue(Func&& function, Args&&... args)
{
	LockGuard              lock(mutex);
	const Function<void()> job = std::bind(std::forward<Func>(function), std::forward<Args>(args)...);
	jobQueue.push_back(job);
}

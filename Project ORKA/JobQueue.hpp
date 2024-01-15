#pragma once

#include "Basics.hpp"
#include "Time.hpp"

using Job = std::function<void()>;

struct JobSystemQueue
{
	void enqueue(const Job& job)
	{
		mutex.lock();
		if(count == jobs.size())
		{
			jobs.push_back(job);
			jobInfo.push_back(0);
		}
		else
		{
			Int tail = head;
			while(jobInfo[tail] != -1) increment(tail);
			jobs[tail] = job;
			jobInfo[tail] = 0;
		}
		count++;
		mutex.unlock();
	}

	void increment(Int & iterator) const
	{
		++iterator;
		if(iterator == jobs.size()) iterator = 0;
	}

	Job dequeue(const Int uniqueThreadID)
	{
		mutex.lock();
		if(count)
		{
			while(jobInfo[head] != 0) increment(head);
			jobInfo[head] = -1;
			Job out = jobs[head];
			count--;
			mutex.unlock();
			return out;
		}
		mutex.unlock();
		return []() { std::this_thread::sleep_for(std::chrono::milliseconds(1));};
	}

	[[nodiscard]] Bool empty() const
	{
		return count <= 0;
	}

	void clear()
	{
		mutex.lock();
		jobs.clear();
		jobInfo.clear();
		count = 0;
		head  = 0;
		mutex.unlock();
	}

	[[nodiscard]] ULL size() const
	{
		return count;
	}

private:
	Mutex mutex;
	Vector<Job>  jobs;
	Vector<Int>  jobInfo;
	Int  head  = 0;		// the first item in the list
	Int  count = 0;		
};
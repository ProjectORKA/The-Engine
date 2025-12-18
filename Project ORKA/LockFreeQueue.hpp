#pragma once

#include "Basics.hpp"

template <typename T, SizeT Size>
struct LockFreeQueue
{
	LockFreeQueue() : head(0), tail(0)
	{
		buffer.resize(Size);
	}

	void enqueue(const T& value)
	{
		SizeT currentTail = tail.load(std::memory_order_relaxed);
		SizeT nextTail;

		do
		{
			nextTail = (currentTail + 1) % Size;

			// Check if the queue is full
			if (nextTail == head.load(std::memory_order_acquire))
			{
				// The queue is full, retry
				continue;
			}

			// Try to update the tail
		}
		while (!tail.compare_exchange_weak(currentTail, nextTail, std::memory_order_relaxed));

		// The tail is updated, now store the value
		buffer[currentTail] = value;
	}

	Bool dequeue(T& result)
	{
		SizeT currentHead = head.load(std::memory_order_relaxed);
		SizeT nextHead;

		do
		{
			nextHead = (currentHead + 1) % Size;

			// Check if the queue is empty
			if (currentHead == tail.load(std::memory_order_acquire))
			{
				// The queue is empty, retry
				return false;
			}

			// Try to update the head
		}
		while (!head.compare_exchange_weak(currentHead, nextHead, std::memory_order_relaxed));

		// The head is updated, now load the value
		result = buffer[currentHead];
		return true;
	}

private:
	Vector<T>          buffer;
	std::atomic<SizeT> head;
	std::atomic<SizeT> tail;
};

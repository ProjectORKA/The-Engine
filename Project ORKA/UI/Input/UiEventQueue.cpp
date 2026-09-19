#include "UiEventQueue.hpp"

namespace Ui
{
	UiEventQueue::UiEventQueue(const SizeT capacity)
		: queueCapacity(std::max(SizeT(1), std::min(capacity, maximumCapacity)))
	{
	}

	Bool UiEventQueue::push(UiEvent event)
	{
		LockGuard lock(mutex);
		if (events.size() >= queueCapacity)
		{
			++droppedEvents;
			return false;
		}
		event.sequence = nextSequence++;
		events.push_back(std::move(event));
		return true;
	}

	Bool UiEventQueue::pop(UiEvent& event)
	{
		LockGuard lock(mutex);
		if (events.empty()) return false;
		event = std::move(events.front());
		events.pop_front();
		return true;
	}

	Vector<UiEvent> UiEventQueue::drain(const SizeT maximum)
	{
		LockGuard lock(mutex);
		const SizeT count = std::min(maximum, events.size());
		Vector<UiEvent> result;
		result.reserve(count);
		for (SizeT i = 0; i < count; ++i)
		{
			result.push_back(std::move(events.front()));
			events.pop_front();
		}
		return result;
	}

	void UiEventQueue::clear()
	{
		LockGuard lock(mutex);
		events.clear();
	}

	SizeT UiEventQueue::size() const
	{
		LockGuard lock(mutex);
		return events.size();
	}

	U64 UiEventQueue::droppedCount() const
	{
		LockGuard lock(mutex);
		return droppedEvents;
	}
}

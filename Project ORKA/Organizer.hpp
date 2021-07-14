#pragma once

#include "Basics.hpp"
#include "Time.hpp"

struct Organizer {
	TimePriorityManager timePriorityManager;
};

struct Task {
	TimePoint lastExecution;
	Float preferredTimeBetweenExecutions;
};

struct TimePriorityManager {
	Vector<Task> tasks;
};
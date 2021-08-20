#pragma once

#include "Basics.hpp"
#include "Time.hpp"



namespace ORGANIZER {

	struct Organizer {
		TimePriorityManager timePriorityManager;
	};

	struct Task {
		Name name = "empty";
		TimePoint lastExecution = std::chrono::steady_clock::now();
		Duration preferredTimeBetweenExecutions = FHours(24);
		Float priority = 0.0;

		void create(Name name, Float repetitionTimeInHours) {
			this->name = name;
			preferredTimeBetweenExecutions = FHours(repetitionTimeInHours);
		};

		void update() {
			FHours h = lastExecution - std::chrono::steady_clock::now();
			priority = h / preferredTimeBetweenExecutions;
		};

		void print() {
			std::cout << priority << "\n";
		}
	};

	struct TimePriorityManager {
		Vector<Task> tasks;
	};
}
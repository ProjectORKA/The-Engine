#pragma once

#include "Basics.hpp"
#include "Game.hpp"


struct Task {
	Name name = "empty";
	TimePoint lastExecution = now();
	Duration preferredTimeBetweenExecutions = FHours(24);
	Float priority = 0.0;

	void create(Name name, Float repetitionTimeInHours) {
		this->name = name;
		preferredTimeBetweenExecutions = FHours(repetitionTimeInHours);
	};

	void update() {
		FHours h = lastExecution - now();
		priority = h / preferredTimeBetweenExecutions;
	};

	void print() {
		std::cout << priority << "\n";
	}
};

struct Organizer : public GameRenderer{
	Vector<Task> tasks;

	void update(Window& window) override {
		for (Task& t : tasks) {
			t.update();
		}
	}
	void render(Window& window, TiledRectangle area) override {
	};
};
#pragma once

#include "Basics.hpp"
#include "Time.hpp"
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

struct Organizer {
	Vector<Task> tasks;

	void update() {
		for (Task& t : tasks) {
			t.update();
		}
	};
};

struct Nizer : public Game {
	using Game::Game;

	Organizer organizer;

	void create() override {
	};
	void update() override {
		organizer.update();
	};
	void render() override {

	};
};
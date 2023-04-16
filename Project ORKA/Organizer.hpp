#pragma once

#include "Basics.hpp"
#include "Game.hpp"

struct User {
	String firstName;
	String lastName;
	String email;
};

struct Task {
	Name name = "empty";
	//UInt priority = -1;
	//Float severity = 0.0;
	//TimePoint lastExecution = now();
	//TimePoint creationDate = now();
	//Duration preferredTimeBetweenExecutions = FHours(24);

};

struct Organizer : public GameRenderer{
	Vector<User> users;
	Vector<Task> tasks;

	void addUser(User user);
	void addTask(Task task);
	void update(Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};
#pragma once

#include "Basics.hpp"
#include "Game.hpp"
#include "Random.hpp"
#include "Player.hpp"


struct Company {
	//primary keys
	Index companyGuid = -1;

	//secondary keys
	String companyName;
	String domain;
};

struct User {
	//primary keys
	Index userGuid = -1;
	Index companyGuid = -1;

	//secondary keys
	String firstName;
	String lastName;
	String email;
	Vec2 position = Vec2(0, 0);
};

struct Task {
	Name name = "empty";
	//UInt priority = -1;
	//Float severity = 0.0;
	//TimePoint lastExecution = now();
	//TimePoint creationDate = now();
	//Duration preferredTimeBetweenExecutions = FHours(24);
};

struct OrganizerRenderer : public GameRenderer {
	DebugPlayer player;
	Float mouseSensitivity = 0.0015f;
	InputEvent enter = InputEvent(InputType::Mouse, LMB, true);
	InputEvent exit = InputEvent(InputType::Mouse, RMB, false);
	InputEvent wireFrameToggle = InputEvent(InputType::KeyBoard, F, true);

	Vector<Company> companies;
	Vector<User> users;
	Vector<Task> tasks;

	Vector<Line3D> connections;

	void addTask(const Task& task);
	void update(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(ResourceManager& resourceManager, Window& window) override;
	void renderConnections(ResourceManager& resourceManager, Renderer& r) const;
	void addCompany(Index guid, const String& companyName, const String& domain);
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
	void addUser(Index userGuid, Index companyGuid, const String& firstName, const String& lastName);
};

struct Organizer {
	UserInterface ui;
	OrganizerRenderer renderer;
	ResourceManager resourceManager;

	void run();
};

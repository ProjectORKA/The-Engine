#pragma once

#include "Basics.hpp"
#include "Game.hpp"
#include "Random.hpp"
#include "Player.hpp"


struct Company {
	//primary keys
	Index companyGUID;

	//secondary keys
	String companyName = "";
	String domain = "";
};

struct User {
	//primary keys
	Index userGUID;
	Index companyGUID;

	//secondary keys
	String firstName;
	String lastName;
	String email;
	Vec2 position;
};

struct Task {
	Name name = "empty";
	//UInt priority = -1;
	//Float severity = 0.0;
	//TimePoint lastExecution = now();
	//TimePoint creationDate = now();
	//Duration preferredTimeBetweenExecutions = FHours(24);

};

struct Connection {
	Vec3 start;
	Vec3 end;
};

struct OrganizerRenderer : public GameRenderer {
	DebugPlayer player;
	Float mouseSensitivity = 0.0015f;
	InputEvent enter = InputEvent(InputType::Mouse, LMB, 1);
	InputEvent exit = InputEvent(InputType::Mouse, RMB, 0);
	InputEvent wireframeToogle = InputEvent(InputType::KeyBoard, F, 1);

	Vector<Company> companies;
	Vector<User> users;
	Vector<Task> tasks;

	std::vector<Connection> connections;

	void addCompany(Index guid, String companyName, String domain) {
		Company company;
		company.companyGUID = guid;
		company.companyName = companyName;
		company.domain = domain;
		companies.pushBack(company);
	};
	void addUser(Index userGUID, Index companyGUID, String firstName, String lastName, String email) {
		User user;
		user.companyGUID = companyGUID;
		user.userGUID = userGUID;
		user.firstName = firstName;
		user.lastName = lastName;
		user.email = email;

		user.position = randomVec2(-100,100);

		users.pushBack(user);
	};
	
	void addTask(Task task);
	void update(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(ResourceManager& resourceManager, Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct Organizer {
	UserInterface ui;
	OrganizerRenderer renderer;
	ResourceManager resourceManager;
	void run() {
		resourceManager.init();

		ui.create();

		ui.window("Organizer", Area(1920, 1080), true, WindowState::windowed, renderer, resourceManager);

		ui.run();
	}
};
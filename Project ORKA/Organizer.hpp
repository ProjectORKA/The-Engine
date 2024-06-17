#pragma once

#include "Basics.hpp"
#include "Game.hpp"
#include "ORKAIntroSequence.hpp"
#include "Player.hpp"

struct Company
{
	// primary keys
	Index companyGuid = -1;

	// secondary keys
	String companyName;
	String domain;
};

struct User
{
	// primary keys
	Index userGuid    = -1;
	Index companyGuid = -1;

	// secondary keys
	String firstName;
	String lastName;
	String email;
	Vec2   position = Vec2(0, 0);
};

struct Task
{
	Name name = "empty";
	// UInt priority = -1;
	// Float severity = 0.0;
	// TimePoint lastExecution = now();
	// TimePoint creationDate = now();
	// Duration preferredTimeBetweenExecutions = FHours(24);
};

struct OrganizerRenderer : GameRenderer
{
	Vector<User>    users;
	Vector<Task>    tasks;
	DebugPlayer     player;
	Vector<Company> companies;
	Vector<Line3D>  connections;
	Float           mouseSensitivity = 0.0015f;
	InputEvent      enter            = InputEvent(InputType::Mouse, LMB, true);
	InputEvent      exit             = InputEvent(InputType::Mouse, RMB, false);
	InputEvent      wireFrameToggle  = InputEvent(InputType::KeyBoard, F, true);

	void addTask(const Task& task);
	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(Window& window) override;
	void renderConnections(Renderer& r) const;
	void addCompany(Index guid, const String& companyName, const String& domain);
	void render(Window& window, TiledRectangle area) override;
	void addUser(Index userGuid, Index companyGuid, const String& firstName, const String& lastName);
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct Organizer
{
	UserInterface     ui;
	ORKAIntroSequence intro;
	Window            window;
	OrganizerRenderer renderer;

	void run()
	{
		ui.create();
		intro.init(renderer);
		window.add(intro);
		ui.window("Organizer", Area(1920, 1080), true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};

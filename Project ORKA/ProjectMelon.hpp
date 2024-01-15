#pragma once

#include "Game.hpp"
#include "KeyMap.hpp"
#include "MelonPlayer.hpp"
#include "MelonWorld.hpp"

struct Window;

// [TODO] maybe implement this stuff
// bullethell
// autofire / direction based dip
// parcour
// time travel self replication challenge
// skillbased
// dash
// morning star
// no movement stops time

// [TODO] next
// vertex colors
// smoke not affected by shadow
// collision
// collectables
// smooth acceleration estimate for particle generation

struct MelonRenderer final : GameRenderer
{
	MelonWorld  world;
	MelonPlayer player;
	Float       mouseSensitivity                = 0.0015f;
	Vec3        mousePosA, mousePosB, mousePosC = Vec3(0);
	InputEvent  exit                            = InputEvent(InputType::Mouse, RMB, false);
	InputEvent  enter                           = InputEvent(InputType::Mouse, LMB, true);

	void update(Window& window) override;
	void create(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void render(Window& window, TiledRectangle area) override;
	void inputEvent(Window& window, InputEvent input) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct ProjectMelon
{
	UserInterface ui;
	Window        window;
	MelonRenderer renderer;

	void run()
	{
		ui.create();
		ui.window("Project Melon", Area(1920, 1080), true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};

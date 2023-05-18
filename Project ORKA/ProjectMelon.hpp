#pragma once

#include "Game.hpp"
#include "KeyMap.hpp"
#include "MelonPlayer.hpp"
#include "MelonWorld.hpp"

struct Window;

//[TODO] maybe implement this stuff
// bullethell
// autofire / direction based dip
// parcour
// time travel self replication challenge
// skillbased
// dash
// morning star
// no movement stops time

//[TODO] next
// vertex colors
// smoke not affected by shadow
// collision
// collectables
// smooth acceleration estimate for particle generation

struct MelonRenderer : public GameRenderer {
	MelonWorld world;
	MelonPlayer player;
	Float mouseSensitivity = 0.0015;
	Vec3 mousePosA, mousePosB, mousePosC = Vec3(0);
	InputEvent exit = InputEvent(InputType::Mouse, RMB, false);
	InputEvent enter = InputEvent(InputType::Mouse, LMB, true);

	void update(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct ProjectMelon {
	ResourceManager resourceManager;
	UserInterface ui;
	MelonRenderer game;

	void run();
};

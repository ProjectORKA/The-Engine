
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
	Float mouseSensitivity = 0.0015;

	Vec3 mousePosA, mousePosB, mousePosC = Vec3(0);

	MelonPlayer player;
	MelonWorld world;

	InputEvent exit = InputEvent(InputType::Mouse, RMB, 0);
	InputEvent enter = InputEvent(InputType::Mouse, LMB, 1);

	void update(Window& window) override;
	void render(Window& window, TiledRectangle area) override;
	void inputEvent(Window& window, InputEvent input) override;
};

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

struct MelonGame : public Game {
	Action	holdPosition;

	Float mouseSensitivity = 0.0015;

	MelonPlayer player;
	MelonWorld world;

	void update() override;
	void render(Renderer& renderer) override;
	void mouseIsMoving(Window& window, IVec2 position);
	void mouseIsScrolled(Window& window, Double xAxis, Double yAxis);
	void mouseIsPressed(Window& window, Int button, Int action, Int modifiers);
	void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers);
};
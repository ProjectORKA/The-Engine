
#pragma once

#include "Game.hpp"
#include "KeyMap.hpp"
#include "MelonPlayer.hpp"
#include "MelonWorld.hpp"
#include "Algorithm.hpp"

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
	Action	holdPosition;

	Float mouseSensitivity = 0.0015;

	Vec3 mousePosA, mousePosB, mousePosC = Vec3(0);

	MelonPlayer player;
	//MelonWorld world;

	void update(Renderer& renderer) override;
	void mouseIsMoving(Window& window, IVec2 position) override;
	void render(TiledRectangle area, Renderer& renderer) override;
	void mouseIsScrolled(Window& window, Double xAxis, Double yAxis)override;
	void buttonIsPressed(Window& window, Key key, Int action, Int modifiers)override;
	void mouseIsPressed(Window& window, MouseButton button, Int action, Int modifiers)override;
};

#pragma once

#include "Basics.hpp"
#include "Game.hpp"
#include "Player.hpp"
#include "KeyMap.hpp"

struct Window;

struct Miniverse : public GameRenderer{
	
	Action	forward;
	Action	backward;
	Action	left;
	Action	right;
	Action	upward;
	Action	downward;
	Action	jump;
	Action	wireframe;

	Float mouseSensitivity = 0.0015f;

	//player
	Player player;

	void mouseIsMoving(Window& window, IVec2 position)  override;
	void render(TiledRectangle area, Renderer& renderer) override;
	void mouseIsScrolled(Window& window, Double xAxis, Double yAxis) override;
	void mouseIsPressed(Window& window, MouseButton button, Int action, Int modifiers) override;
	void buttonIsPressed(Window& window, Key key, Int action, Int modifiers) override;
};
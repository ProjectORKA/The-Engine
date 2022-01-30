
#pragma once

#include "Basics.hpp"
#include "Game.hpp"
#include "Player.hpp"
#include "KeyMap.hpp"

struct Window;

struct Miniverse : public Game{
	
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

	void update() override;
	void render(Renderer& renderer) override;
	void mouseIsMoving(Window& window, IVec2 position)  override;
	void mouseIsScrolled(Window& window, Double xAxis, Double yAxis) override;
	void mouseIsPressed(Window& window, Int button, Int action, Int modifiers) override;
	void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) override;
};
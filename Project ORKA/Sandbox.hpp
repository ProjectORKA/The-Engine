
#pragma once

#include "Game.hpp"
#include "Basics.hpp"
#include "KeyMap.hpp"
#include "Player.hpp"

struct Renderer;
struct Window;

#include "NeighborQuadtree.hpp"
#include "DynameshTerrain.hpp"

struct Sandbox : public Game {
	
	Mutex mutex;

	Action	forward;
	Action	backward;
	Action	left;
	Action	right;
	Action	upward;
	Action	downward;
	Float mouseSensitivity = 0.0015f;
	Player player;

	NeighborQuadtree tree;
	DynameshTerrain dmt;


	Sandbox() {
		//dmt.create();
		tree.create();
	}
	~Sandbox() {
		tree.destroy();
	}

	void render(Renderer& renderer) override;
	void mouseIsMoving(Window& window, IVec2 position)  override;
	void mouseIsPressed(Window& window, Int button, Int action, Int modifiers) override;
	void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) override;
};
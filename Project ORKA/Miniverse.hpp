
#pragma once

#include "Basics.hpp"
#include "Game.hpp"
#include "Player.hpp"
#include "KeyMap.hpp"
#include "Transform.hpp"
#include "Random.hpp"

struct Window;

#define MINIVERSE_TREE_COUNT 1024

struct MiniverseSimulation : public GameSimulation {
	Vector<Vec4> treeTransforms;

	MiniverseSimulation() {
		treeTransforms.resize(MINIVERSE_TREE_COUNT);
		for (Vec4& t : treeTransforms) {
			t.x = randomFloat(0.0f, 100.0f);
			t.y = randomFloat(0.0f, 100.0f);
			t.z = 0.0f;
			t.w = randomFloat(0.5f, 1.2f);
		}
	}
};

struct Miniverse : public GameRenderer{
	
	MiniverseSimulation* gameSimulation = nullptr;

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

	Miniverse(MiniverseSimulation& simulation) {
		gameSimulation = &simulation;
	}
	Miniverse() = delete;

	void mouseIsMoving(Window& window, IVec2 position)  override;
	void render(TiledRectangle area, Renderer& renderer) override;
	void mouseIsScrolled(Window& window, Double xAxis, Double yAxis) override;
	void mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers) override;
	void buttonIsPressed(Window& window, Key key, ActionState action, Int modifiers) override;
};
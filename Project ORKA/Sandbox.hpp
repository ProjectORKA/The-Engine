
#pragma once

#include "Game.hpp"
#include "Basics.hpp"
#include "KeyMap.hpp"
#include "Player.hpp"

//#include "Renderer.hpp"
struct Renderer;
struct Window;

#include "NeighborQuadtree.hpp"
#include "DynameshTerrain.hpp"

#include "SphereMeshing.hpp"


struct Sandbox : public GameRenderer {
	
	Mutex mutex;

	Action	forward;
	Action	backward;
	Action	left;
	Action	right;
	Action	upward;
	Action	downward;
	Float mouseSensitivity = 0.0015f;
	Player player;


	SphereMeshing sm;

	Sandbox() {
		//dmt.create();
		//tree.create();
	}
	~Sandbox() {
		//tree.destroy();
	}

	void render(TiledRectangle area, Renderer& renderer) override;
	void mouseIsMoving(Window& window, IVec2 position)  override;
	void mouseIsScrolled(Window& window, Double xAxis, Double yAxis) override;;
	void mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers) override;
	void buttonIsPressed(Window& window, Key key, ActionState action, Int modifiers) override;
};

#pragma once

#include "Game.hpp"
#include "Basics.hpp"
#include "KeyMap.hpp"
#include "Player.hpp"

//#include "Renderer.hpp"
struct Renderer;
struct Window;

#include "NeighbourQuadtree.hpp"
#include "NeighbourOctree.hpp"
#include "DynameshTerrain.hpp"
#include "SphereMeshing.hpp"
#include "GeometryClipmapTerrain.hpp"
#include "LevelledGraph.hpp"
#include "Molecules2D.hpp"

struct SandboxSimulation : public GameSimulation {
	//ClipMapTerrain terr;
	Vec3 location = Vec3(0);

	void update() override;;
	SandboxSimulation();
	~SandboxSimulation();
};

struct Sandbox : public GameRenderer {
	
	Mutex mutex;
	Player player;
	Action forward;
	Action backward;
	Action left;
	Action right;
	Action upward;
	Action downward;
	Float mouseSensitivity = 0.0015f;
	SandboxSimulation * simulation = nullptr;

	LevelledGraph system;

	Sandbox(SandboxSimulation& sim);
	void update(Renderer& renderer) override;
	void mouseIsMoving(Window& window, IVec2 position)  override;
	void render(TiledRectangle area, Renderer& renderer) override;
	void mouseIsScrolled(Window& window, Double xAxis, Double yAxis) override;
	void buttonIsPressed(Window& window, Key key, ActionState action, Int modifiers) override;
	void mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers) override;
};
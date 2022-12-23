
#pragma once

#include "Game.hpp"
#include "Basics.hpp"
#include "KeyMap.hpp"
#include "Player.hpp"

//#include "Renderer.hpp"
struct Renderer;
struct Window;

//#include "NeighbourQuadtree.hpp"
//#include "NeighbourOctree.hpp"
//#include "DynameshTerrain.hpp"
//#include "SphereMeshing.hpp"
//#include "Molecules2D.hpp"
//#include "MindMap.hpp"

struct Sandbox {
	void run();
};

struct SandboxRenderer : public GameRenderer {
	Mutex mutex;
	DebugPlayer player;
	Float mouseSensitivity = 0.0015f;
	Bool wireframeMode = false;

	GPUMesh gpuMesh;
	CPUMesh cpuMesh;

	InputEvent exit = InputEvent(InputType::Mouse, RMB, 0);
	InputEvent enter = InputEvent(InputType::Mouse, LMB, 1);
	InputEvent toggleWireframe = InputEvent(InputType::KeyBoard, F, 1);

	void create(Window& window) override;
	void update(Window& window) override;
	void render(Window& window, TiledRectangle area) override;
	void inputEvent(Window& window, InputEvent input) override;
};
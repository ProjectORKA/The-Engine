#pragma once

#include "UserInterface.hpp"
#include "Game.hpp"
#include "Player.hpp"

//prototypes
//#include "KDTreePrototype.hpp"
//#include "WeirdSphereAlgorithm.hpp"
//#include "KDTreeImplementation.hpp"
//#include "AxisNeighborPrototype.hpp"
//#include "DelaunayTriangulation.hpp"
//#include "AxisNeighborPrototype2.hpp"
//#include "TriangulationPrototype.hpp"
//#include "SquareNeighborPrototype.hpp"
//#include "PersistentInfoPrototype.hpp"
#include "LabyrinthGeneratorPrototype.hpp"
//#include "ClosestConnectionPrototype.hpp"
//#include "ParallelizedConvexHullPrototype.hpp"

struct PrototypingRenderer final : GameRenderer
{
	Mutex       mutex;
	DebugPlayer player;
	Framebuffer framebuffer;
	Bool        wireframeCube    = false;
	Float       mouseSensitivity = 0.0015f;
	InputId     repeatAction     = InputId(InputType::KeyBoard, Space);
	InputEvent  enter            = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent  exit             = InputEvent(InputType::Mouse, Rmb, false);
	InputEvent  wireframeToggle  = InputEvent(InputType::KeyBoard, F, true);
	InputEvent  action           = InputEvent(InputType::KeyBoard, G, true);

	// prototypes
	// KDTreePrototype prototype;
	// KDTreePrototype2 prototype;
	// AxisNeighborPrototype prototype;
	// AxisNeighborPrototype2 prototype;
	// TriangulationPrototype prototype;
	// PersistentInfoPrototype prototype;
	// SquareNeighborPrototype prototype;
	LabyrinthGeneratorPrototype prototype;
	// PlaneIntersectionPrototype prototype;
	// ClosestConnectionPrototype prototype;
	// WeirdSphereAlgorithmPrototype prototype;
	// ParallelizedConvexHullPrototype prototype;

	void update(Window& window) override;
	void create(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void render(Window& window, TiledRectangle area) override;
	void inputEvent(Window& window, InputEvent input) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct PrototypingSandbox
{
	PrototypingRenderer renderer;

	void run()
	{
		ui.window("Prototyping Sandbox", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};

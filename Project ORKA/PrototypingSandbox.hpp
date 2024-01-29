#pragma once

#include "UserInterface.hpp"
#include "Game.hpp"
#include "Player.hpp"

//prototypes
#include "AxisNeighborPrototype.hpp"
#include "AxisNeighborPrototype2.hpp"
#include "SquareNeighborPrototype.hpp"
#include "KDTreePrototype.hpp"
#include "WeirdSphereAlgorithm.hpp"
#include "KDTreeImplementation.hpp"
#include "ParallelizedConvexHullPrototype.hpp"

struct PlaneIntersectionPrototype : Prototype
{
	void create() override;
	void action() override;
	void render(Renderer& r, const Player& player) override;
};

struct PrototypingRenderer final : GameRenderer
{
	Mutex       mutex;
	DebugPlayer player;
	Framebuffer framebuffer;
	Float       mouseSensitivity = 0.0015f;
	InputId     repeatAction     = InputId(InputType::KeyBoard, SPACE);
	InputEvent  enter            = InputEvent(InputType::Mouse, LMB, true);
	InputEvent  exit             = InputEvent(InputType::Mouse, RMB, false);
	InputEvent  wireframeToggle  = InputEvent(InputType::KeyBoard, F, true);
	InputEvent  action           = InputEvent(InputType::KeyBoard, G, true);

	// prototypes
	//PlaneIntersectionPrototype prototype;
	//AxisNeighborPrototype prototype;
	AxisNeighborPrototype2 prototype;
	//SquareNeighborPrototype prototype;
	//WeirdSphereAlgorithmPrototype prototype;
	//KDTreePrototype prototype;
	//KDTreePrototype2 prototype;
	//ParallelizedConvexHullPrototype prototype;

	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(Window& window) override;
	void render(Window& window, TiledRectangle area) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct PrototypingSandbox
{
	UserInterface       ui;
	Window              window;
	PrototypingRenderer renderer;

	void run()
	{
		ui.create();
		ui.window("Prototyping Sandbox", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};

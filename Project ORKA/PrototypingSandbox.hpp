#pragma once

#include "UserInterface.hpp"
#include "Game.hpp"
#include "Player.hpp"

//prototypes
#include "AxisNeighborPrototype.hpp"

struct PlaneIntersectionPrototype : Prototype
{
	void create() override;
	void action() override;
	void render(Renderer& r, ResourceManager& rm, const Player& player) override;
};

struct PrototypingRenderer final : GameRenderer
{
	Mutex       mutex;
	DebugPlayer player;
	Framebuffer framebuffer;
	Float       mouseSensitivity = 0.0015f;
	InputEvent  enter            = InputEvent(InputType::Mouse, LMB, true);
	InputEvent  exit             = InputEvent(InputType::Mouse, RMB, false);
	InputEvent  wireframeToggle  = InputEvent(InputType::KeyBoard, F, true);
	InputEvent  action           = InputEvent(InputType::KeyBoard, SPACE, true);

	// prototypes
	//PlaneIntersectionPrototype prototype;
	AxisNeighborPrototype prototype;

	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(ResourceManager& resourceManager, Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct PrototypingSandbox
{
	UserInterface       ui;
	PrototypingRenderer renderer;
	ResourceManager     resourceManager;

	void run();
};

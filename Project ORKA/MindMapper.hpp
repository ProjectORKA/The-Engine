#pragma once

#include "Game.hpp"
#include "JobSystem.hpp"
#include "MindMap.hpp"
#include "Player.hpp"

void simWork(MindMap& system);

struct MindMapperSim final : GameSimulation
{
	MindMap   system;
	const Int nodeCount = 100;

	void destroy() override;
	void update(Float delta) override;
	void create(ResourceManager& resourceManager) override;
};

struct MindMapperRenderer final : GameRenderer
{
	DebugPlayer    player;
	Bool           grow       = false;
	MindMapperSim* sim        = nullptr;
	InputEvent     enter      = InputEvent(InputType::Mouse, 0, true);
	InputEvent     exit       = InputEvent(InputType::Mouse, 1, false);
	InputEvent     growToggle = InputEvent(InputType::KeyBoard, SPACE, false);

	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(ResourceManager& resourceManager, Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct MindMapper
{
	UserInterface      ui;
	MindMapperSim      sim;
	MindMapperRenderer renderer;
	ResourceManager    resourceManager;
	void               run();
};

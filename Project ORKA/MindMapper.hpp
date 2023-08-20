#pragma once

#include "Game.hpp"
#include "MindMap.hpp"
#include "Player.hpp"

struct MindMapperRenderer final : GameRenderer
{
	MindMap     system;
	DebugPlayer player;
	Bool        grow       = false;
	InputEvent  enter      = InputEvent(InputType::Mouse, 0, true);
	InputEvent  exit       = InputEvent(InputType::Mouse, 1, false);
	InputEvent  growToggle = InputEvent(InputType::KeyBoard, SPACE, false);

	MindMapperRenderer() = default;
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
	ResourceManager    resourceManager;
	MindMapperRenderer mindMapperRenderer;
	void               run();
};

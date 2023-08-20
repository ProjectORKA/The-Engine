#pragma once

#include "Game.hpp"

struct ImguiWrapperRenderer : GameRenderer
{
	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(ResourceManager& resourceManager, Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct ImguiExample
{
	UserInterface        ui;
	ImguiWrapperRenderer renderer;
	ResourceManager      resourceManager;

	void run();
};

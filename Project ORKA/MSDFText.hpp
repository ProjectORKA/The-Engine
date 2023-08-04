#pragma once

#include "Game.hpp"

struct MSDFTextRenderer : GameRenderer
{
	void update(Window& window) override;
	void destroy(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(ResourceManager& resourceManager, Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct MSDFText
{
	UserInterface    ui;
	ResourceManager  resourceManager;
	MSDFTextRenderer msdfTextRenderer;

	void run();
};

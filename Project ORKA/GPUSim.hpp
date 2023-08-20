#pragma once

#include "Game.hpp"
#include "ResourceManager.hpp"

struct GPUSimRenderer final : GameRenderer
{
	Framebuffer framebuffer1;
	Framebuffer framebuffer2;
	Bool        flipFlop = true;
	const Int   worldSize = 1024;

	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void create(ResourceManager& rm, Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void render(ResourceManager& rm, Window& window, TiledRectangle area) override;
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct GPUSim
{
	UserInterface   ui;
	GPUSimRenderer  gpuSimRenderer;
	ResourceManager resourceManager;

	void run();
};

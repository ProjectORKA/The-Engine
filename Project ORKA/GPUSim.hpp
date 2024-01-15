#pragma once

#include "Game.hpp"

struct GPUSimRenderer final : GameRenderer
{
	Framebuffer framebuffer1;
	Framebuffer framebuffer2;
	Bool        flipFlop  = true;
	const Int   worldSize = 1024;

	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void create(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void render(Window& window, TiledRectangle area) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct GPUSim
{
	UserInterface  ui;
	Window         window;
	GPUSimRenderer renderer;

	void run()
	{
		ui.create();
		ui.window("ORKA GPU Simulation", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};

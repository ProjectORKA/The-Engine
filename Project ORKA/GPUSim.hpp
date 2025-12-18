#pragma once

#include "Game.hpp"

struct GPUSimRenderer final : GameRenderer
{
	Framebuffer simulationFramebuffer;
	Bool        flipFlop  = true;
	const Int   worldSize = 1024;

	void update(Window& window) override;
	void create(Window& window) override;
	void renderScene(Window& window) override;
	void destroy(Window& window) override;
	void renderInteractive(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
};

struct GPUSim
{
	GPUSimRenderer renderer;

	void run()
	{
		ui.window("ORKA GPU Simulation", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};

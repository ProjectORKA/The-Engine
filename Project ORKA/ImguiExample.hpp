#pragma once

#include "Game.hpp"

struct ImguiWrapperRenderer : GameRenderer
{
	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(Window& window) override;
	void render(Window& window, TiledRectangle area) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct ImguiExample
{
	UserInterface        ui;
	ImguiWrapperRenderer renderer;
	Window               window;

	void run()
	{
		ui.create();
		ui.window("Imgui Wrapper", Area(1920, 1080), true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};

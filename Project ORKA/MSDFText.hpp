#pragma once

#include "Game.hpp"

struct MSDFTextRenderer : GameRenderer
{
	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(Window& window) override;
	void render(Window& window, TiledRectangle area) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct MSDFText
{
	MSDFTextRenderer renderer;

	void run()
	{
		ui.window("MSDF Text Rendering", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};

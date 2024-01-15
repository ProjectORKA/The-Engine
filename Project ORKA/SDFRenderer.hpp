#pragma once

#include "Game.hpp"
#include "Player.hpp"

struct SignedDistanceFieldRenderer final : GameRenderer
{
	DebugPlayer player;
	Framebuffer framebuffer;
	InputEvent  enter         = InputEvent(InputType::Mouse, LMB, true);
	InputEvent  exit          = InputEvent(InputType::Mouse, RMB, false);
	InputEvent  reloadShaders = InputEvent(InputType::KeyBoard, T, false);

	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(Window& window) override;
	void render(Window& window, TiledRectangle area) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct SDFRenderer
{
	UserInterface               ui;
	Window                      window;
	SignedDistanceFieldRenderer renderer;

	void run()
	{
		ui.create();
		ui.window("SDFRenderer", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};

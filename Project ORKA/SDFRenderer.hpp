#pragma once
#include "Framebuffer.hpp"
#include "Game.hpp"
#include "Player.hpp"

struct SignedDistanceFieldRenderer final : GameRenderer
{
	DebugPlayer player;
	InputEvent  enter         = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent  exit          = InputEvent(InputType::Mouse, Rmb, false);
	InputEvent  reloadShaders = InputEvent(InputType::KeyBoard, T, false);

	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(Window& window) override;
	void renderScene(Window& window) override;
	void renderInteractive(Window& window) override;
};

struct SdfRenderer
{
	SignedDistanceFieldRenderer renderer;

	void run()
	{
		ui.window("SDFRenderer", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};

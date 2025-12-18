#pragma once

#include "Game.hpp"
#include "Player.hpp"

struct AIPlaygroundRenderer final : GameRenderer
{
	DebugPlayer          player;
	//Network here

	InputEvent enter           = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent exit            = InputEvent(InputType::Mouse, Rmb, false);
	InputEvent wireframeToggle = InputEvent(InputType::KeyBoard, F, true);

	void update(Window& window) override;
	void create(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void renderScene(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void renderInteractive(Window& window) override;
};

struct AIPlayground
{
	AIPlaygroundRenderer renderer;

	void run()
	{
		ui.window("ORKA AI Playground", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};

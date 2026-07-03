#pragma once
#include "Game.hpp"
#include "Player.hpp"

struct SandboxRenderer final : GameRenderer
{
	Mutex       mutex;
	DebugPlayer player;
	Float       mouseSensitivity = 0.0015f;
	InputEvent  enter            = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent  exit             = InputEvent(InputType::Mouse, Rmb, false);
	InputEvent  wireframeToggle  = InputEvent(InputType::KeyBoard, F, true);

	void create(Window& window) override;
	void update(Window& window) override;
	void destroy(Window& window) override;
	void renderScene(Window& window) override;
	void renderInteractive(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent& input) override;
};

struct Sandbox
{
	SandboxRenderer renderer;

	void run()
	{
		Window& window = ui.window("ORKA Sandbox", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed);
		window.addElement(renderer);
		ui.run();
	}
};

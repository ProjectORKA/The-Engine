#pragma once

#include "UserInterface.hpp"
#include "Game.hpp"
#include "Player.hpp"
#include "SineWaveTerrainRegressor.hpp"

struct PrototypingRenderer final : GameRenderer
{
	Mutex       mutex;
	DebugPlayer player;
	Bool        actionToggle     = false;
	Bool        wireframeCube    = true;
	Float       mouseSensitivity = 0.0015f;
	InputId     repeatAction     = InputId(InputType::KeyBoard, Space);
	InputEvent  enter            = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent  exit             = InputEvent(InputType::Mouse, Rmb, false);
	InputEvent  wireframeToggle  = InputEvent(InputType::KeyBoard, F, true);
	InputEvent  action           = InputEvent(InputType::KeyBoard, G, true);
	InputEvent  toggleAction     = InputEvent(InputType::KeyBoard, T, true);

	SineWaveTerrainRegressor prototype;

	void update(Window& window) override;
	void create(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void renderScene(Window& window) override;
	void inputEvent(Window& window, InputEvent& input) override;
	void renderInteractive(Window& window) override;
};

struct PrototypingSandbox
{
	PrototypingRenderer renderer;

	void run()
	{
		ui.window("Prototyping Sandbox", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};

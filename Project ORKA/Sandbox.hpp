
#pragma once

#include "Game.hpp"
#include "Player.hpp"

#include "KeyMap.hpp"

struct SandboxRenderer : public GameRenderer {
	Mutex mutex;
	DebugPlayer player;
	Float mouseSensitivity = 0.0015f;

	InputEvent enter = InputEvent(InputType::Mouse, LMB, 1);
	InputEvent exit = InputEvent(InputType::Mouse, RMB, 0);
	InputEvent wireframeToogle = InputEvent(InputType::KeyBoard, F, 1);

	void create(Engine& engine, Window& window) {
		player.camera.location = Vec3(0, -5, 0);
	};
	void update(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void render(Engine & engine, Window& window, TiledRectangle area) override;
};

struct Sandbox {
	Engine engine;
	UserInterface ui;
	SandboxRenderer sandboxRenderer;

	void run() {
		ui.window("ORKA Sandbox", Area(1920, 1080), true, WindowState::maximized, sandboxRenderer, engine);
		ui.run();
	};
};

#pragma once

#include "Game.hpp"
#include "Player.hpp"

#include "KeyMap.hpp"

struct SandboxRenderer : public GameRenderer {
	Mutex mutex;
	Player player;
	Float mouseSensitivity = 0.0015f;

	InputEvent enter = InputEvent(InputType::Mouse, LMB, 1);
	InputEvent exit = InputEvent(InputType::Mouse, RMB, 0);
	InputEvent wireframeToogle = InputEvent(InputType::KeyBoard, F, 1);

	void update(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void render(Engine & engine, Window& window, TiledRectangle area) override;
};

struct Sandbox {
	SandboxRenderer sandboxRenderer;

	Sandbox(Engine& engine);
};
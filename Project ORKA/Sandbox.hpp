#pragma once

#include "Game.hpp"
#include "Player.hpp"
#include "KeyMap.hpp"
#include "ResourceManager.hpp"

struct SandboxRenderer : public GameRenderer {
	Mutex mutex;
	DebugPlayer player;
	Float mouseSensitivity = 0.0015f;

	InputEvent enter = InputEvent(InputType::Mouse, LMB, true);
	InputEvent exit = InputEvent(InputType::Mouse, RMB, false);
	InputEvent wireframeToogle = InputEvent(InputType::KeyBoard, F, true);

	void update(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(ResourceManager& resourceManager, Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct Sandbox {
	UserInterface ui;
	ResourceManager resourceManager;
	SandboxRenderer sandboxRenderer;

	void run();
};

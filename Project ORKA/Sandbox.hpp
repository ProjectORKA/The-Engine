
#pragma once

#include "Game.hpp"
#include "Player.hpp"
#include "KeyMap.hpp"
#include "ResourceManager.hpp"

struct SandboxRenderer : public GameRenderer {
	Mutex mutex;
	DebugPlayer player;
	Float mouseSensitivity = 0.0015f;

	InputEvent enter = InputEvent(InputType::Mouse, LMB, 1);
	InputEvent exit = InputEvent(InputType::Mouse, RMB, 0);
	InputEvent wireframeToogle = InputEvent(InputType::KeyBoard, F, 1);

	void update(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(ResourceManager& resourceManager, Window& window);
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct Sandbox {
	UserInterface ui;
	ResourceManager resourceManager;
	SandboxRenderer sandboxRenderer;

	void run();
};
#pragma once

#include "Game.hpp"
#include "Player.hpp"
#include "KeyMap.hpp"
#include "ResourceManager.hpp"

struct SandboxRenderer final : GameRenderer
{
	Mutex       mutex;
	DebugPlayer player;
	Framebuffer framebuffer;
	Float       mouseSensitivity = 0.0015f;
	InputEvent  enter            = InputEvent(InputType::Mouse, LMB, true);
	InputEvent  exit             = InputEvent(InputType::Mouse, RMB, false);
	InputEvent  wireframeToggle  = InputEvent(InputType::KeyBoard, F, true);

	void update(Window& window) override;
	void destroy(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(ResourceManager& resourceManager, Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct Sandbox
{
	UserInterface   ui;
	SandboxRenderer renderer;
	ResourceManager resourceManager;

	void run();
};

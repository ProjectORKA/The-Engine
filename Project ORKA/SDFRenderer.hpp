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
	void inputEvent(Window& window, InputEvent input) override;
	void create(ResourceManager& resourceManager, Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct SDFRenderer
{
	UserInterface               ui;
	ResourceManager             resourceManager;
	SignedDistanceFieldRenderer blobRenderer;

	void run();
};

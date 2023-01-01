
#pragma once

#include "Game.hpp"
#include "Basics.hpp"
#include "KeyMap.hpp"
#include "Player.hpp"
#include "UIButton.hpp"

struct Renderer;
struct Window;

struct Toggle : public UIButton {
	Bool toggle = false;

	void doThis() override;
	void update(Window& window) override;;
};

struct UISandbox : public GameRenderer {
	Bool paused = false;
	Toggle pauseButton;
	UIButton saveButton;

	UISandbox();
	void update(Window& window)override;
	void inputEvent(Window& window, InputEvent input) override;
	void render(Engine& engine, Window& window, TiledRectangle area) override;
	void renderInteractive(Engine& engine, Window& window, TiledRectangle area) override;
};
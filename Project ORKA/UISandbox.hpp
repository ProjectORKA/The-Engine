
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

	void doThis() override {
		toggle = !toggle;
	}
};

struct UISandbox : public GameRenderer {
	Bool paused = false;
	Toggle pauseButton;
	UIButton saveButton;

	UISandbox();
	void render(Window& window, TiledRectangle area) override;
	void inputEvent(Window& window, InputEvent input) override;
	void mouseMoved(Window& window, MouseMovementInput input)  override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};
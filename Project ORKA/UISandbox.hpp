
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

	UISandbox() {
		pauseButton.padding(50);
	}

	void mouseIsMoving(Window& window, IVec2 position)  override;
	void render(TiledRectangle area, Renderer& renderer) override;
	void renderInteractive(TiledRectangle area, Renderer& renderer) override;
	void buttonIsPressed(Window& window, Key key, ActionState action, Int modifiers) override;
	void mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers) override;
};
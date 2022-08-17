
#pragma once

#include "UIElement.hpp"

struct UIButton : public UIElement {
	Bool pressed = false;
	UIElement* content = nullptr;

	UIButton();
	UIButton& insert(UIElement& element);
	virtual void doThis() { beep(); };

	void update(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void render(Window& window, TiledRectangle renderArea) override;
	void renderInteractive(Window& window, TiledRectangle renderArea) override;
};
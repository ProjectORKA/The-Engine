
#pragma once

#include "UIElement.hpp"

struct UIButton : public UIElement {
	Bool pressed = false;
	UIElement* content = nullptr;

	UIButton();
	UIButton& insert(UIElement& element);
	virtual void doThis() {beep();};
	void render(TiledRectangle renderArea, Renderer& renderer)override;
	void renderInteractive(TiledRectangle renderArea, Renderer& renderer)override;
	void mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers)override;
};
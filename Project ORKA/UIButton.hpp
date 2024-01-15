#pragma once

#include "UIElement.hpp"

struct UIButton : UIElement
{
	Bool       pressed = false;
	UIElement* content = nullptr;

	UIButton();
	virtual void doThis();
	UIButton&    insert(UIElement& element);
	void         update(Window& window) override;
	void         destroy(Window& window) override;
	void         inputEvent(Window& window, InputEvent input) override;
	void         create(Window& window) override;
	void         render(Window& window, TiledRectangle renderArea) override;
	void         renderInteractive(Window& window, TiledRectangle renderArea) override;
};

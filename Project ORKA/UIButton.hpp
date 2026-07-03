#pragma once

#include "UIElement.hpp"

struct UIButton : UIElement
{
	Bool       pressed = false;
	Bool       hovered = false;
	UIElement* content = nullptr;
	Bool       hasHover = false;

	Color  hoveredBackgroundColor = Color(1.0, 1.0, 1.0, 0.2);
	Color  activeBackgroundColor = Color(1.0, 1.0, 1.0, 0.5);

	UIButton();
	virtual void doThis();
	UIButton&    insert(UIElement& element);
	//void         update(Window& window) override;
	void         create(Window& window) override;
	void         destroy(Window& window) override;
	void         renderInteractive(Window& window) override;
	void         inputEvent(Window& window, InputEvent& input) override;
	void         onMouseEnter(Window& window) override;
	void         onMouseLeave(Window& window) override;
};

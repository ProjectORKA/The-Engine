
#pragma once

#include "UIElement.hpp"

struct UIText : public UIElement{
	String text;
	UIText();
};

void renderUIText(UIElement* element, Window& window, TiledRectangle screenArea);
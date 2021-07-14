
#pragma once

#include "UIElement.hpp"

void renderUISplit(UIElement* element, Window& window, TiledRectangle screenArea);

struct UISplit : public UIElement {
	Bool vertical = true;

	UISplit();
};
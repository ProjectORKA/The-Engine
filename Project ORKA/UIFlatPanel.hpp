
#pragma once

#include "UIElement.hpp"

struct UIFlatPanel : public UIElement {
	UIFlatPanel();
};
void createUIFlatPanel(UIElement* element, Window& window);
void renderUIFlatPanel(UIElement* element, Window& window, TiledRectangle screenArea);
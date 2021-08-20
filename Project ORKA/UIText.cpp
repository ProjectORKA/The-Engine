#include "UIText.hpp"

UIText::UIText() {
	createFunction = createUIElement;
	renderFunction = renderUIText;
	destroyFunction = destroyUIElement;
	updateFunction = updateUIElement;
}

void renderUIText(UIElement* element, Window& window, TiledRectangle screenArea) {
	FontStyle style;
	style.size = 128;
	style.letterSpacing = 0.5;
	window.renderer.textRenderSystem.render(reinterpret_cast<UIText* >(element)->text.c_str(), element->posX, element->posY, style);
}
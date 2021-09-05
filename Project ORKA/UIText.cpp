#include "UIText.hpp"

UIText::UIText() {
	createFunction = createUIElement;
	renderFunction = renderUIText;
	destroyFunction = destroyUIElement;
	updateFunction = updateUIElement;
}

void renderUIText(UIElement* element, Window& window, TiledRectangle screenArea) {
	window.renderer.textRenderSystem.render(reinterpret_cast<UIText* >(element)->text.c_str(), element->posX, element->posY, fonts.paragraph);
}
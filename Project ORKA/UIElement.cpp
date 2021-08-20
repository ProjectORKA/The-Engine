
#include "UIElement.hpp"
#include "Window.hpp"

//base functions
void UIElement::destroy(Window & window) {
	destroyFunction(this, window);
}
void UIElement::create(Window& window) {
	createFunction(this,window);
}
void UIElement::update(Window& window) {
	updateFunction(this, window);
}
void UIElement::render(Window& window, TiledRectangle screenArea) {
	renderFunction(this, window, screenArea);
}

void UIElement::createChildren(Window & window)
{
	for (UIElement* element : content) {
		element->create(window);
	}
}
void UIElement::updateChildren(Window& window)
{
	for (UIElement* element : content) {
		element->update(window);
	}
}
void UIElement::renderChildren(Window& window, TiledRectangle screenArea)
{
	for (UIElement* element : content) {
		element->render(window, screenArea);
	}
}
void UIElement::destroyChildren(Window& window)
{
	for (UIElement* element : content) {
		element->destroy(window);
	}
}

UIElement::UIElement() {
	static Index idCounter = 0;
	idCounter++;
	id = idCounter;

	destroyFunction = destroyUIElement;
	updateFunction = updateUIElement;
	createFunction = createUIElement;
	renderFunction = renderUIElement;
}

//basic ui functions
void destroyUIElement(UIElement * element, Window& window) {
	for (UIElement * elemPtr : element->content) {
		elemPtr->destroy(window);
	}
	element->content.clear();
}
void createUIElement(UIElement* element, Window & window) {
	for (UIElement* elemPtr : element->content) {
		elemPtr->create(window);
	}
}
void updateUIElement(UIElement* element, Window& window) {
	for (UIElement* elemPtr : element->content) {
		elemPtr->update(window);
	}
}
void renderUIElement(UIElement* element, Window & window, TiledRectangle screenArea) {
	for (UIElement* elemPtr : element->content) {
		elemPtr->render(window, screenArea);
	}
}





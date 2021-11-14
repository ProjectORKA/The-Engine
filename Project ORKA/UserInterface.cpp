
#include "Window.hpp"
#include "UserInterface.hpp"

void UserInterface::destroy(Window& window) {
	for (UIElement* element : contents) {
		element->destroy(window);
		delete element;
	}
	contents.clear();
}

void UserInterface::render(Window& window) {
	if (contents.size() > 0) {
		contents.front()->update(window);
		contents.front()->render(window, TiledRectangle(window.getWindowContentSize()));
	}
}

void UserInterface::buttonIsPressed(Window & window, Int keyID, Int action, Int modifiers) {
	
	for (UIElement* element : contents) {
		element->buttonIsPressed(window, keyID, action, modifiers);
	}
}

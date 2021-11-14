
#pragma once

#include "UIElement.hpp"
#include "UIGameView.hpp"
#include "UIFlatPanel.hpp"

struct Window;

struct UserInterface {
	List<UIElement*> contents;
	UIElement* focusedElement = nullptr;

	void render(Window& window);
	void destroy(Window& window);
	void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers);
};
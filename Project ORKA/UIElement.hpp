
#pragma once

#include "Renderer.hpp"
#include "InputManager.hpp"
#include "GameSimulation.hpp"
#include "Functor.hpp"
#include "UIConstraints.hpp"

struct Window;

struct UIElement {
	Index id = 0;
	UIElement* parent = nullptr;
	Vector<UIElement *> content;
	Vector<UIConstraint> constraints;

	void create(Window& window);
	void update(Window& window);
	void destroy(Window & window);
	void render(Window& window, TiledRectangle screenArea);
	void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers);

	void (*createFunction)(UIElement* element, Window& window) = nullptr;
	void (*updateFunction)(UIElement* element, Window& window) = nullptr;
	void (*destroyFunction)(UIElement* element, Window& window) = nullptr;
	void (*renderFunction)(UIElement* element, Window& window, TiledRectangle screenArea) = nullptr;
	void (*buttonIsPressedFunction)(UIElement* element, Window& window, Int keyID, Int action, Int modifiers) = nullptr;

	void createChildren(Window& window);
	void updateChildren(Window& window);
	void destroyChildren(Window& window);
	void renderChildren(Window& window, TiledRectangle screenArea);
	void buttonIsPressedChildren(Window& window, Int keyID, Int action, Int modifiers);

	UIElement();
};
void destroyUIElement(UIElement* element, Window & window);
void createUIElement(UIElement* element, Window& window);
void updateUIElement(UIElement* element, Window& window);
void renderUIElement(UIElement* element, Window& window, TiledRectangle screenArea);

#include "Window.hpp"
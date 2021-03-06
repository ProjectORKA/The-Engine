
#pragma once

#include "Renderer.hpp"
#include "InputManager.hpp"
#include "GameSimulation.hpp"
#include "Functor.hpp"
#include "UIConstraints.hpp"

struct Window;

struct UIElement {
	UIElement* parent = nullptr;
	Vector<UIElement *> content;
	Vector<UIConstraint> constraints;


	Int sizeX = 100;
	Int sizeY = 100;
	Int posX = 0;
	Int posY = 0;

	void create(Window& window);
	void update(Window& window);
	void destroy(Window & window);
	void render(Window& window, TiledRectangle screenArea);

	void (*createFunction)(UIElement* element, Window& window) = nullptr;
	void (*updateFunction)(UIElement* element, Window& window) = nullptr;
	void (*destroyFunction)(UIElement* element, Window& window) = nullptr;
	void (*renderFunction)(UIElement* element, Window& window, TiledRectangle screenArea) = nullptr;

	void createChildren(Window& window);
	void updateChildren(Window& window);
	void destroyChildren(Window& window);
	void renderChildren(Window& window, TiledRectangle screenArea);

	UIElement();
};
void destroyUIElement(UIElement* element, Window & window);
void createUIElement(UIElement* element, Window& window);
void updateUIElement(UIElement* element, Window& window);
void renderUIElement(UIElement* element, Window& window, TiledRectangle screenArea);

#include "Window.hpp"
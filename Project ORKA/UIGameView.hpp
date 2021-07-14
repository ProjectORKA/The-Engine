#pragma once

#include "Basics.hpp"
#include "UIElement.hpp"
#include "Renderer.hpp"

struct Window;

void createUIORKALogo(UIElement* element, Window& window);
void renderUIORKALogo(UIElement* element, Window& window, TiledRectangle screenArea);

struct UIORKALogo : public UIElement {
	UIORKALogo();
};

void createUIORKAGame(UIElement* element, Window& window);
void renderUIORKAGame(UIElement* element, Window& window, TiledRectangle screenArea);

struct UIORKAGame : public UIElement {
	UIORKAGame();
};

void createUIORKA3DPlayground(UIElement* element, Window& window);
void renderUIORKA3DPlayground(UIElement* element, Window& window, TiledRectangle screenArea);

struct UIORKA3DPlayground : public UIElement {
	UIORKA3DPlayground();
};
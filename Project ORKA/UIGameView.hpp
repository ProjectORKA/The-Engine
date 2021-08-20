#pragma once

#include "Basics.hpp"
#include "UIElement.hpp"
#include "Renderer.hpp"

struct Window;

extern Map<Index, PlanetCamera> planetCameras;
extern Map<Index, SimpleCamera> cameras;

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
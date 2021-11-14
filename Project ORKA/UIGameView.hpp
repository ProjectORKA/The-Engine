#pragma once

#include "Basics.hpp"
#include "UIElement.hpp"
#include "Game.hpp"

struct Window;

void renderUIGameView(UIElement* element, Window& window, TiledRectangle screenArea);
void updateUIGameView(UIElement* element, Window& window);

struct UIGameView : public UIElement {
	PlanetCamera planetCamera;
	SimpleCamera camera;
	Game * game = nullptr;


	UIGameView(Game* game, Window & window) {
		this->game = game;

		renderFunction = &renderUIGameView;
		updateFunction = &updateUIGameView;
	}
};
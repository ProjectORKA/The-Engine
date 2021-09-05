#pragma once

#include "Basics.hpp"
#include "UIElement.hpp"
#include "Game.hpp"

//
//extern Map<Index, PlanetCamera> planetCameras;
//extern Map<Index, SimpleCamera> cameras;
//
//void createUIORKALogo(UIElement* element, Window& window);
//void renderUIORKALogo(UIElement* element, Window& window, TiledRectangle screenArea);
//
//struct UIORKALogo : public UIElement {
//	UIORKALogo();
//};
//
//void createUIORKAGame(UIElement* element, Window& window);
//void renderUIORKAGame(UIElement* element, Window& window, TiledRectangle screenArea);
//
//struct UIORKAGame : public UIElement {
//	UIORKAGame();
//};

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
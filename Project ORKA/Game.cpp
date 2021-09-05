
#include "Game.hpp"
#include "Window.hpp"

Game::Game(Window& window) {
	window.contents.push_back(new UIGameView(this, window));
	window.contents.back()->create(window);
}

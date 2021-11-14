
#include "Game.hpp"
#include "Window.hpp"

Game::Game(Window& window) {
	window.userInterface.contents.push_back(new UIGameView(this, window));
	window.userInterface.contents.back()->create(window);
}

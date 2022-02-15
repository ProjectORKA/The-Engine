
#pragma once

#include "Game.hpp"
#include "FileSystem.hpp"

struct Sortr : public GameRenderer {
	Path unsortedFolder;
	
	Vector<Path> sortFolders;

	void render(TiledRectangle area, Renderer & renderer) override {


	};

	void buttonIsPressed(Window& window, Key key, ActionState action, Int modifiers) override {
	
	};
};

void removeDoubleQuotes(String& s);
void sortr();
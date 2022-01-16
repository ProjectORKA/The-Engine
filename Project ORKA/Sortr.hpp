
#pragma once

#include "Game.hpp"
#include "FileSystem.hpp"

struct Sortr : public Game {
	Path unsortedFolder;
	
	Vector<Path> sortFolders;

	void render(Renderer & renderer) override {


	};

	void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) override {
	
	};
};

void removeDoubleQuotes(String& s);
void sortr();

#pragma once

#include "Game.hpp"

struct Sortr : public Game {
	Path unsortedFolder;
	
	Vector<Path> sortFolders;

	void render(Renderer & renderer) override {


	};

	void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) override {
	
	};
};

#include <filesystem>
#include <fstream>
#include <iostream>

void removeDoubleQuotes(String& s);

void sortr();
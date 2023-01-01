
#pragma once

#include "Game.hpp"
#include "FileSystem.hpp"

struct Sortr : public GameRenderer {
	Path unsortedFolder;
	
	Vector<Path> sortFolders;

	void render(Engine & engine, Window& window, TiledRectangle area) override {};
};

void removeDoubleQuotes(String& s);
void sortr();
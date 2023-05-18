#pragma once

#include "Game.hpp"
#include "FileSystem.hpp"

struct Sortr : public GameRenderer {
	Path unsortedFolder;

	Vector<Path> sortFolders;

	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override {};
};

void removeDoubleQuotes(String& s);
void sortr();

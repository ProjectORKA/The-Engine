
#pragma once

#include "Game.hpp"
#include "Engine.hpp"

//this "game" plays and intro sequence and then replaces itself with any game you want

struct ORKAIntroSequence : public GameRenderer {
	Bool decorated = true;								//weather the apps window is decoreted or not
	Bool initializedRenderer = false;
	GameRenderer* gameStartingAfterIntro = nullptr;

	void init(GameRenderer& game);
	void render(Engine & engine, Window& window, TiledRectangle area) override;
};
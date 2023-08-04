#pragma once

#include "Game.hpp"
#include "ResourceManager.hpp"

// this "game" plays and intro sequence and then replaces itself with any game you want

struct ORKAIntroSequence : GameRenderer
{
	Bool          decorated              = true; // weather the apps window is decoreted or not
	Bool          initializedRenderer    = false;
	GameRenderer* gameStartingAfterIntro = nullptr;

	void init(GameRenderer& game);
	void update(Window& window) override;
	void destroy(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(ResourceManager& resourceManager, Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

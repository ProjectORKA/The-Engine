#pragma once

#include "Game.hpp"

// this "game" plays and intro sequence and then replaces itself with any game you want

struct ORKAIntroSequence : GameRenderer
{
	Bool          decorated              = true; // weather the apps window is decoreted or not
	Bool          initializedRenderer    = false;
	GameRenderer* gameStartingAfterIntro = nullptr;

	void init(GameRenderer& game);
	void update(Window& window) override;
	void create(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void render(Window& window, TiledRectangle area) override;
	void inputEvent(Window& window, InputEvent input) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

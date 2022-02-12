#pragma once

#include "Game.hpp"

struct Window;

struct AIPlayground : public GameRenderer {
	void render(TiledRectangle area, Renderer& renderer) override;
};
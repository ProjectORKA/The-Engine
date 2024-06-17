#pragma once

#include "Player.hpp"

struct Renderer;

struct PlanetSystemPlayer : DebugPlayer
{
	UllVec3 chunkPosition = UllVec3(0);
	void    render(Window& window) override;
};

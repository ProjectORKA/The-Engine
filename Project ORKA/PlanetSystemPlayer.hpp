#pragma once
#include "Player.hpp"

struct Window;

struct PlanetSystemPlayer : DebugPlayer
{
	UllVec3 chunkPosition = UllVec3(0);
	void    render(Window& window) override;
};

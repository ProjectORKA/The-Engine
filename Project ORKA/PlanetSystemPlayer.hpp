#pragma once

#include "Player.hpp"

struct Renderer;

struct PlanetSystemPlayer : DebugPlayer
{
	ULLVec3 chunkLocation = ULLVec3(0);
	void    render(ResourceManager& resourceManager, Window& window) override;
};

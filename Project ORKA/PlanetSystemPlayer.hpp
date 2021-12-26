
#pragma once

#include "Player.hpp"

struct Renderer;

struct PlanetSystemPlayer : public Player {
	ULLVec3 chunkLocation = ULLVec3(ULLONG_MAX / 2);

	void render(Renderer& renderer) override;
};
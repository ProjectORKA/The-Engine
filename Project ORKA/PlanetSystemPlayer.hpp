
#pragma once

#include "Player.hpp"

struct Renderer;

struct PlanetSystemPlayer : public DebugPlayer {
	ULLVec3 chunkLocation = ULLVec3(0);
	void render(Engine& engine, Window & window) override;
};
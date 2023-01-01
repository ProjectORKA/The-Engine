
#pragma once

#include "Basics.hpp"
#include "Engine.hpp"

struct Renderer;

struct SDFTerrainRenderSystem {
	void render(Engine& engine, Renderer& renderer);
};
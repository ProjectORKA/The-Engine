#pragma once

#include "Basics.hpp"
#include "Framebuffer.hpp"
#include "OctreeSystemRenderer.hpp"
#include "QuadtreeSystemRenderer.hpp"

struct Renderer;
struct PlanetSystem;
struct PlanetSystemPlayer;

struct PlanetRenderSystem
{
	QuadtreeRenderSystem quadtreeRenderSystem;
	Bool                 chunkBorders    = true;
	Bool                 worldDistortion = true;
	Bool                 vertexColors    = false;

	void destroy(Renderer& renderer);
	void update(Renderer& renderer, const PlanetSystem& planetSystem, PlanetSystemPlayer& player);
	void renderAllLevels(PlanetSystem& planetSystem, Renderer& renderer, const PlanetSystemPlayer& player, const Framebuffer& framebuffer);
	void renderLevel(PlanetSystem& planetSystem, Renderer& renderer, const PlanetSystemPlayer& player, UShort level, const Framebuffer& framebuffer);
};

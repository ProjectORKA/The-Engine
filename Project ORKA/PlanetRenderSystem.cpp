
#include "PlanetRenderSystem.hpp"
#include "Renderer.hpp"

void PlanetRenderSystem::create(Renderer & renderer)
{
	quadtreeRenderSystem.create(renderer);
}
void PlanetRenderSystem::destroy()
{
	octreeRenderSystem.destroy();
	quadtreeRenderSystem.destroy();
}
void PlanetRenderSystem::render(PlanetSystem& planetSystem, Renderer& renderer, PlanetCamera & planetCamera)
{
	quadtreeRenderSystem.render(planetSystem, renderer, planetCamera);
}
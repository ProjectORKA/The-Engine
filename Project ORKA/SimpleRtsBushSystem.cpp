#include "SimpleRtsBushSystem.hpp"
#include "SimpleRts.hpp"

void SimpleRtsBushSystem::update(SimpleRtsSimulation& sim) {}

void SimpleRtsBushSystem::destroy(const SimpleRtsSimulation& sim) {}

void SimpleRtsBushSystem::render(Renderer& renderer) const
{
	renderer.renderMeshInstanced("Bush", positions);
}

void SimpleRtsBushSystem::create(const SimpleRtsSimulation& sim)
{
	for (int i = 0; i < count; i++)
	{
		spawnEntity(sim);
	}
}

void SimpleRtsBushSystem::spawnEntity(const SimpleRtsSimulation& sim)
{
	positions.push_back(sim.getRandomSpawnPos(radius));
}

Bool SimpleRtsBushSystem::doesCollide(const Vec2 position, const Float radius) const
{
	return circleCollidingWithCircles(position, radius, positions, radius);
}

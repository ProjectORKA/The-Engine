#include "SimpleRtsBushSystem.hpp"
#include "SimpleRts.hpp"

void SimpleRtsBushSystem::update(SimpleRtsSimulation& sim) {}

void SimpleRtsBushSystem::destroy(const SimpleRtsSimulation& sim)
{
	positions.clear();
}

void SimpleRtsBushSystem::setPosition(const SimpleRtsSimulation& sim, const Index index, const Vec2 position)
{
	positions[index] = Vec3(position.x, position.y, sim.getTerrainHeight(position));
}

Vec3 SimpleRtsBushSystem::getPosition(const Index index) const
{
	return positions[index];
}

void SimpleRtsBushSystem::render(Renderer& renderer)
{
	renderer.renderMeshInstanced("Bush", positions);
}

void SimpleRtsBushSystem::create(const SimpleRtsSimulation& sim)
{
	name = "BushSystem";
	const Int spawnCount = sim.worldSettings.bushes;
	if (debugSimpleRts) logDebug("Spawning " + std::to_string(spawnCount) + " bushes");
	for (Int i = 0; i < spawnCount; i++) spawnEntity(sim);
	if (debugSimpleRts) logDebug("Spawned bushes");
}

void SimpleRtsBushSystem::spawnEntity(const SimpleRtsSimulation& sim)
{
	positions.push_back(sim.getRandomSpawnPos(radius));
}

Bool SimpleRtsBushSystem::doesCollide(const Vec2 position, const Float radius) const
{
	return circleCollidingWithCircles(position, radius, positions, radius);
}

Int SimpleRtsBushSystem::getCount() const
{
	return static_cast<Int>(positions.size());
}

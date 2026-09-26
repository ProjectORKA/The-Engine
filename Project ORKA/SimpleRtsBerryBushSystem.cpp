#include "SimpleRtsBerryBushSystem.hpp"
#include "Random.hpp"
#include "SimpleRts.hpp"

void SimpleRtsBerryBushSystem::spawnEntity(const SimpleRtsSimulation& sim)
{
	positions.emplace_back(sim.getRandomSpawnPos(radius));
}

void SimpleRtsBerryBushSystem::render(Renderer& renderer)
{
	mutex.lock();
	renderer.renderMeshInstanced("BerryBush", positions);
	mutex.unlock();
}

void SimpleRtsBerryBushSystem::create(const SimpleRtsSimulation& sim)
{
	name = "BerryBushSystem";
	const Int spawnCount = sim.worldSettings.berryBushes;
	if (debugSimpleRts) logDebug("Spawning " + std::to_string(spawnCount) + " berry bushes");
	mutex.lock();
	positions.clear();
	positions.reserve(static_cast<SizeT>(spawnCount));
	mutex.unlock();
	for (Int i = 0; i < spawnCount; i++) spawnEntity(sim);
	if (debugSimpleRts) logDebug("Spawned berry bushes");
}

Bool SimpleRtsBerryBushSystem::doesCollide(const Vec2 position, const Float radius) const
{
	return circleCollidingWithCircles(position, radius, positions, radius);
}

void SimpleRtsBerryBushSystem::update(SimpleRtsSimulation& sim) {}

void SimpleRtsBerryBushSystem::eat(const Index berryBushId, SimpleRtsSimulation& sim)
{
	const Index bushId     = randomIntFast(sim.bushSystem.getCount());
	const Vec2  position   = positions[berryBushId];
	positions[berryBushId] = sim.bushSystem.getPosition(bushId);
	sim.bushSystem.setPosition(sim, bushId, position);
}

void SimpleRtsBerryBushSystem::destroy(const SimpleRtsSimulation& sim)
{
	mutex.lock();
	positions.clear();
	mutex.unlock();
}

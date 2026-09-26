#include "SimpleRtsBodySystem.hpp"
#include "SimpleRTS.hpp"

void SimpleRtsBodySystem::update(SimpleRtsSimulation& sim) {}

void SimpleRtsBodySystem::spawnEntity(const SimpleRtsSimulation& sim)
{
	for (auto& p : positions) p.z -= 1.0f / static_cast<Float>(max(1, sim.worldSettings.bodies));
	positions.push_back(sim.getRandomSpawnPos());
}

void SimpleRtsBodySystem::render(Renderer& renderer)
{
	renderer.renderMeshInstanced("Body", positions);
}

void SimpleRtsBodySystem::create(const SimpleRtsSimulation& sim)
{
	name = "BodySystem";
	const Int spawnCount = sim.worldSettings.bodies;
	if (debugSimpleRts) logDebug("Spawning " + std::to_string(spawnCount) + " bodies");
	for (Int i = 0; i < spawnCount; i++) spawnEntity(sim);
	if(debugSimpleRts) logDebug("Spawned bodies");
}

void SimpleRtsBodySystem::destroy(const SimpleRtsSimulation& sim)
{
	positions.clear();
}

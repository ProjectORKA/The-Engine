#include "SimpleRtsBodySystem.hpp"
#include "SimpleRTS.hpp"

void SimpleRtsBodySystem::update(SimpleRtsSimulation& sim) {}

void SimpleRtsBodySystem::spawnEntity(const SimpleRtsSimulation& sim)
{
	for (auto& p : positions) p.z -= 1.0f / static_cast<Float>(count);
	positions.push_back(sim.getRandomSpawnPos());
}

void SimpleRtsBodySystem::render(Renderer& renderer)
{
	renderer.renderMeshInstanced("Body", positions);
}

void SimpleRtsBodySystem::create(const SimpleRtsSimulation& sim)
{
	name = "BodySystem";
	if(debugSimpleRts) logDebug("Spawning " + std::to_string(count) + " bodies");
	for (int i = 0; i < count; i++) spawnEntity(sim);
	if(debugSimpleRts) logDebug("Spawned bodies");
}

void SimpleRtsBodySystem::destroy(const SimpleRtsSimulation& sim) {}

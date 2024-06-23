#include "SimpleRtsBodySystem.hpp"
#include "SimpleRTS.hpp"

void SimpleRtsBodySystem::update(SimpleRtsSimulation& sim) {}

void SimpleRtsBodySystem::spawnEntity(const SimpleRtsSimulation& sim)
{
	for (auto& p : positions) p.z -= 1.0f / static_cast<Float>(count);
	positions.push_back(Vec3(sim.getRandomSpawnPos(radius), 0));
}

void SimpleRtsBodySystem::render(Renderer& renderer) const
{
	renderer.renderMeshInstanced("Body", positions);
}

void SimpleRtsBodySystem::create(const SimpleRtsSimulation& sim)
{
	for (int i = 0; i < count; i++) spawnEntity(sim);
}

void SimpleRtsBodySystem::destroy(const SimpleRtsSimulation& sim) {}

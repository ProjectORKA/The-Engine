#include "SimpleRtsBerryBushSystem.hpp"
#include "Random.hpp"
#include "SimpleRts.hpp"

void SimpleRtsBerryBushSystem::spawnEntity(const SimpleRtsSimulation& sim)
{
	positions.push_back(sim.getRandomSpawnPos(radius));
}

void SimpleRtsBerryBushSystem::render(Renderer& renderer) const
{
	renderer.renderMeshInstanced("BerryBush", positions);
}

void SimpleRtsBerryBushSystem::create(const SimpleRtsSimulation& sim)
{
	for (int i = 0; i < count; i++) spawnEntity(sim);
}

void SimpleRtsBerryBushSystem::eat(const Index berryBushId, SimpleRtsSimulation& sim)
{
	const Index bushId               = randomIntFast(sim.bushSystem.count);
	const Vec2  position             = positions[berryBushId];
	positions[berryBushId]           = sim.bushSystem.positions[bushId];
	sim.bushSystem.positions[bushId] = position;
}

void SimpleRtsBerryBushSystem::destroy(const SimpleRtsSimulation& sim) {}

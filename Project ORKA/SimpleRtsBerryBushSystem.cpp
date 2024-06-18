#include "SimpleRtsBerryBushSystem.hpp"
#include "Random.hpp"
#include "SimpleRts.hpp"

void SimpleRtsBerryBushSystem::spawnEntity(const SimpleRtsSimulation& sim)
{
	positions.push_back(sim.getRandomSpawnPos(getEntityRadius()));
}

Name SimpleRtsBerryBushSystem::getEntityName() const
{
	return "BerryBush";
}

Int SimpleRtsBerryBushSystem::getEntityCount() const
{
	return 3000;
}

Float SimpleRtsBerryBushSystem::getEntityRadius() const
{
	return 1.0f;
}

void SimpleRtsBerryBushSystem::eat(const Index berryBushId, SimpleRtsSimulation& sim) {
	const Index bushId               = randomIntFast(sim.bushSystem.getEntityCount());
	const Vec2  position             = positions[berryBushId];
	positions[berryBushId]           = sim.bushSystem.positions[bushId];
	sim.bushSystem.positions[bushId] = position;
}

#include "SimpleRtsBushSystem.hpp"
#include "SimpleRts.hpp"

void SimpleRtsBushSystem::spawnEntity(const SimpleRtsSimulation& sim)
{
	positions.push_back(sim.getRandomSpawnPos(getEntityRadius()));
}

Name SimpleRtsBushSystem::getEntityName() const
{
	return "Bush";
}

Int SimpleRtsBushSystem::getEntityCount() const
{
	return 3000;
}

Float SimpleRtsBushSystem::getEntityRadius() const
{
	return 1.0f;
}

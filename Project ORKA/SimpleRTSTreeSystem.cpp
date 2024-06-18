#include "SimpleRTSTreeSystem.hpp"
#include "Random.hpp"
#include "Renderer.hpp"
#include "SimpleRTS.hpp"

void SimpleRtsTreeSystem::render(Renderer& renderer) const
{
	renderer.useShader("simpleRTS");
	renderer.renderMeshInstanced("Tree", positions, directions, scales);
}

Float SimpleRtsTreeSystem::calculateTreeSize(const UInt id) const
{
	return maxHeights[id] / pow((1 + pow((ages[id] / minTreeLifeExpectancy), -3.651294)), 0.417522); //maxHeights[id] * (ages[id] / lifeExpectancies[id]);
}

void SimpleRtsTreeSystem::destroy(const SimpleRtsSimulation& sim)
{
	ages.clear();
	scales.clear();
	maxHeights.clear();
	positions.clear();
	directions.clear();
	lifeExpectancies.clear();
	treeCount = 0;
}

void SimpleRtsTreeSystem::reset(const SimpleRtsSimulation& sim)
{
	destroy(sim);
	create(sim);
}

void SimpleRtsTreeSystem::create(const SimpleRtsSimulation& sim)
{
	while (positions.size() < count) spawnTree(sim);
}

void SimpleRtsTreeSystem::update(SimpleRtsSimulation& sim)
{
	timer.start();
	for (UInt i = 0; i < treeCount; i++)
	{
		ages[i] += sim.timeStep();
		scales[i] = calculateTreeSize(i);
		if (ages[i] >= lifeExpectancies[i]) cutTree(sim, i);
	}
	timer.stop();
}

void SimpleRtsTreeSystem::spawnTree(const SimpleRtsSimulation& sim)
{
	positions.push_back(sim.getRandomSpawnPos(treeRadius));
	lifeExpectancies.push_back(randomFloatFast(minTreeLifeExpectancy, maxTreeLifeExpectancy));
	ages.push_back(randomFloatFast(0, minTreeLifeExpectancy));
	directions.push_back(randomUnitVec2Fast());
	maxHeights.push_back(randomFloatFast(minGrownTreeHeight, maxGrownTreeHeight));
	scales.push_back(calculateTreeSize(treeCount));
	treeCount++;
}

bool SimpleRtsTreeSystem::doesCollide(const Vec2 position, const Float radius) const
{
	return circleCollidingWithCircles(position, radius, positions, treeRadius);
}

void SimpleRtsTreeSystem::cutTree(const SimpleRtsSimulation& sim, const UInt id)
{
	scales[id] = 0;
	ages[id]   = 0;

	if (true)
	{
		Vec2 newTreePosition = positions[randomIntFast(positions.size())];
		loopWithinCentered(newTreePosition, sim.mapSize);
		while (pointWithinDistanceOfOtherPoints(newTreePosition, positions, treeRadius))
		{
			newTreePosition += randomPointInCircleFast(treeSpread);
			loopWithinCentered(newTreePosition, sim.mapSize);
		}
		positions[id] = newTreePosition;
	}
	else
	{
		// [TODO] WIP
		Vec2 newTreePosition = sim.getRandomSpawnPos(treeRadius);
		positions[id]        = newTreePosition;
	}
}

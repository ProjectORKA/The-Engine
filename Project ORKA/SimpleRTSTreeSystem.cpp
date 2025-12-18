#include "SimpleRTSTreeSystem.hpp"
#include "Random.hpp"
#include "Renderer.hpp"
#include "SimpleRTS.hpp"

void SimpleRtsTreeSystem::render(Renderer& renderer)
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
	name = "TreeSystem";
	if (debugSimpleRts) logDebug("Creating " + toString(count) + " trees.");
	while (positions.size() < count) spawnTree(sim);
	if (debugSimpleRts) logDebug("Created trees.");
}

void SimpleRtsTreeSystem::update(SimpleRtsSimulation& sim)
{
	for (UInt i = 0; i < treeCount; i++)
	{
		ages[i] += sim.timeStep();
		scales[i] = calculateTreeSize(i);
		if (ages[i] >= lifeExpectancies[i]) cutTree(sim, i);
	}
}

void SimpleRtsTreeSystem::spawnTree(const SimpleRtsSimulation& sim)
{
	positions.emplace_back(sim.getRandomSpawnPos(treeRadius));
	lifeExpectancies.emplace_back(randomFloatFast(minTreeLifeExpectancy, maxTreeLifeExpectancy));
	ages.emplace_back(randomFloatFast(0, minTreeLifeExpectancy));
	directions.emplace_back(randomUnitVec2Fast());
	maxHeights.emplace_back(randomFloatFast(minGrownTreeHeight, maxGrownTreeHeight));
	scales.emplace_back(calculateTreeSize(treeCount));
	treeCount++;
}

void SimpleRtsTreeSystem::forceSpawnTree(const SimpleRtsSimulation& sim, Vec2 position)
{
	positions.emplace_back(position, sim.getTerrainHeight(position));
	lifeExpectancies.emplace_back(randomFloatFast(minTreeLifeExpectancy, maxTreeLifeExpectancy));
	ages.emplace_back(randomFloatFast(0, minTreeLifeExpectancy));
	directions.emplace_back(randomUnitVec2Fast());
	maxHeights.emplace_back(randomFloatFast(minGrownTreeHeight, maxGrownTreeHeight));
	scales.emplace_back(calculateTreeSize(treeCount));
	treeCount++;
}

Bool SimpleRtsTreeSystem::doesCollide(const Vec2 position, const Float radius) const
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
		while (pointWithinDistanceOfOtherPoints(newTreePosition, positions.toVec2(), treeRadius))
		{
			newTreePosition += randomPointInCircleFast(treeSpread);
			loopWithinCentered(newTreePosition, sim.mapSize);
		}
		positions[id] = Vec3(newTreePosition, sim.getTerrainHeight(newTreePosition));
	}
	else
	{
		// [TODO] WIP
		Vec2 newTreePosition = sim.getRandomSpawnPos(treeRadius);
		positions[id]        = Vec3(newTreePosition, sim.getTerrainHeight(newTreePosition));
	}
}

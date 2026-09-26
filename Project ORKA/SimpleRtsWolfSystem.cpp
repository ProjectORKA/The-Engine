#include "SimpleRtsWolfSystem.hpp"
#include "Random.hpp"
#include "SimpleRTS.hpp"

void SimpleRtsWolfSystem::spawnEntity(const SimpleRtsSimulation& sim)
{
	ages.push_back(randomFloatFast(0.0f, rabbitMaxAge));
	const Vec3 spawnPos = sim.getRandomSpawnPos();
	positions.push_back(Vec2(spawnPos.x, spawnPos.y));
	renderPositions.push_back(spawnPos);
	directions.push_back(randomUnitVec2Fast());
	sizes.push_back(sizeFromAge(ages.back()));
	speeds.push_back(speedFromAge(ages.back()));
}

void SimpleRtsWolfSystem::update(SimpleRtsSimulation& sim)
{
	for (Int i = 0; i < spawnCount; i++)
	{
		ages[i] += sim.timeStep();
	}

	for (Int i = 0; i < spawnCount; i++)
	{
		if (ages[i] >= rabbitMaxAge) killWolf(i);
	}

	for (Int i = 0; i < spawnCount; i++)
	{
		sizes[i]  = sizeFromAge(ages[i]);
		speeds[i] = speedFromAge(ages[i]);
	}

	for (Int i = 0; i < spawnCount; i++) directions[i] = normalize(directions[i] + randomUnitVec2Fast() * jitter);
	for (Int i = 0; i < spawnCount; i++)
	{
		Vec2 pos = positions[i] + directions[i] * sim.timeStep() * speeds[i];
		loopWithinCentered(pos, sim.worldSettings.mapSize);
		positions[i] = pos;
		renderPositions[i] = Vec3(pos, sim.getTerrainHeight(pos));
	}
}

Float SimpleRtsWolfSystem::sizeFromAge(const Float age) const
{
	return lerp(rabbitMinSize, rabbitMaxSize, age / rabbitMaxAge);
}

Float SimpleRtsWolfSystem::speedFromAge(const Float age) const
{
	return lerp(minSpeed, maxSpeed, age / rabbitMaxAge);
}

Bool SimpleRtsWolfSystem::doesCollide(const Vec2 position, const Float radius) const
{
	return false; //circleCollidingWithCircles(position, radius, positions, rabbitRadius);
}

void SimpleRtsWolfSystem::create(const SimpleRtsSimulation& sim)
{
	name = "RabbitSystem";
	spawnCount = sim.worldSettings.wolves;
	if (debugSimpleRts) logDebug("Spawning " + std::to_string(spawnCount) + " wolves");
	for (Int i = 0; i < spawnCount; i++) spawnEntity(sim);
	if (debugSimpleRts) logDebug("Spawned wolves");
}

void SimpleRtsWolfSystem::destroy(const SimpleRtsSimulation& sim)
{
	sizes.clear();
	ages.clear();
	positions.clear();
	renderPositions.clear();
	directions.clear();
	speeds.clear();
	spawnCount = 0;
}

void SimpleRtsWolfSystem::killWolf(const Int id)
{
	Index parentID = randomIntFast(0, max(0, spawnCount - 1));
	ages[id]       = 0.0f;
	speeds[id]     = minSpeed;
	sizes[id]      = rabbitMinSize;
	positions[id]  = positions[parentID];
	renderPositions[id] = renderPositions[parentID];
	directions[id] = randomUnitVec2Fast();
}

void SimpleRtsWolfSystem::render(Renderer& renderer)
{
	renderer.renderMeshInstanced("Wolf", renderPositions, directions, sizes);
}

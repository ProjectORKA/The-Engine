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
	for (Int i = 0; i < count; i++)
	{
		ages[i] += sim.timeStep();
	}

	for (Int i = 0; i < count; i++)
	{
		if (ages[i] >= rabbitMaxAge) killWolf(i);
	}

	for (Int i = 0; i < count; i++)
	{
		sizes[i]  = sizeFromAge(ages[i]);
		speeds[i] = speedFromAge(ages[i]);
	}

	for (Int i = 0; i < count; i++) directions[i] = normalize(directions[i] + randomUnitVec2Fast() * jitter);
	for (Int i = 0; i < count; i++)
	{
		Vec2 pos = positions[i] + directions[i] * sim.timeStep() * speeds[i];
		loopWithinCentered(pos, sim.mapSize);
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
	if (debugSimpleRts) logDebug("Spawning " + std::to_string(count) + " wolves");
	for (Int i = 0; i < count; i++) spawnEntity(sim);
	if (debugSimpleRts) logDebug("Spawned wolves");
}

void SimpleRtsWolfSystem::destroy(const SimpleRtsSimulation& sim) {}

void SimpleRtsWolfSystem::killWolf(const Int id)
{
	Index parentID = randomIntFast(0, count - 1);
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

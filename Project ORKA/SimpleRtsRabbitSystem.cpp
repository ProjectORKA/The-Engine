#include "SimpleRtsRabbitSystem.hpp"
#include "Random.hpp"
#include "SimpleRTS.hpp"

void SimpleRtsRabbitSystem::spawnEntity(const SimpleRtsSimulation& sim)
{
	const Float age = randomFloatFast(0.0f, rabbitMaxAge);
	ages.emplace_back(age);
	states.emplace_back(Standing);
	sizes.emplace_back(sizeFromAge(age));
	speeds.emplace_back(speedFromAge(age));
	directions.emplace_back(randomUnitVec2Fast());
	const Vec3 spawnPos = sim.getRandomSpawnPos();
	positions.emplace_back(spawnPos.x, spawnPos.y);
	renderPositions.emplace_back(spawnPos);
	waitTimes.emplace_back(sim.totalTime);
}

void SimpleRtsRabbitSystem::update(SimpleRtsSimulation& sim)
{
	// age
	for (Int i = 0; i < spawnCount; i++)
	{
		ages[i] += sim.timeStep();
	}

	for (Int i = 0; i < spawnCount; i++)
	{
		if (ages[i] >= rabbitMaxAge) killRabbit(i);
	}

	for (Int i = 0; i < spawnCount; i++)
	{
		sizes[i]  = sizeFromAge(ages[i]);
		speeds[i] = speedFromAge(ages[i]);
	}

	for (Int i = 0; i < spawnCount; i++)
	{
		switch (states[i])
		{
		case Standing: if (waitTimes[i] < sim.totalTime)
			{
				waitTimes[i] = sim.totalTime + randomFloatFast(1, 10);
				states[i]    = Hopping;
			}
			break;
		case Hopping:
			{
				directions[i] = normalize(directions[i] + randomUnitVec2Fast() * jitter);
				Vec2 pos      = positions[i] + directions[i] * sim.timeStep() * speeds[i] * hopSpeed;
				loopWithinCentered(pos, sim.worldSettings.mapSize);
				positions[i] = pos;
				renderPositions[i] = Vec3(pos, sim.getTerrainHeight(pos));

				if (waitTimes[i] < sim.totalTime)
				{
					waitTimes[i] = sim.totalTime + randomFloatFast(1, 10);
					states[i]    = Standing;
				}
			}
			break;
		case Running:
			{
				directions[i] = normalize(directions[i] + randomUnitVec2Fast() * jitter);
				Vec2 pos      = positions[i] + directions[i] * sim.timeStep() * speeds[i];
				loopWithinCentered(pos, sim.worldSettings.mapSize);
				positions[i] = pos;
				renderPositions[i] = Vec3(pos, sim.getTerrainHeight(pos));
			}
			break;
		}
	}
}

Float SimpleRtsRabbitSystem::sizeFromAge(const Float age) const
{
	return lerp(rabbitMinSize, rabbitMaxSize, age / rabbitMaxAge);
}

Float SimpleRtsRabbitSystem::speedFromAge(const Float age) const
{
	return lerp(minSpeed, maxSpeed, age / rabbitMaxAge);
}

void SimpleRtsRabbitSystem::create(const SimpleRtsSimulation& sim)
{
	name = "RabbitSystem";
	spawnCount = sim.worldSettings.rabbits;
	if (debugSimpleRts) logDebug("Spawning " + std::to_string(spawnCount) + " rabbits");
	for (Int i = 0; i < spawnCount; i++) spawnEntity(sim);
	if (debugSimpleRts) logDebug("Spawned rabbits");
}

void SimpleRtsRabbitSystem::destroy(const SimpleRtsSimulation& sim)
{
	ages.clear();
	states.clear();
	sizes.clear();
	speeds.clear();
	directions.clear();
	positions.clear();
	renderPositions.clear();
	waitTimes.clear();
	spawnCount = 0;
}

Bool SimpleRtsRabbitSystem::doesCollide(const Vec2 position, const Float radius) const
{
	return circleCollidingWithCircles(position, radius, positions, rabbitRadius);
}

void SimpleRtsRabbitSystem::killRabbit(const Int id)
{
	Index parentID = randomIntFast(0, max(0, spawnCount - 1));
	ages[id]       = 0.0f;
	speeds[id]     = minSpeed;
	sizes[id]      = rabbitMinSize;
	positions[id]  = positions[parentID];
	renderPositions[id] = renderPositions[parentID];
	directions[id] = randomUnitVec2Fast();
}

void SimpleRtsRabbitSystem::render(Renderer& renderer)
{
	renderer.renderMeshInstanced("Rabbit", renderPositions, directions, sizes);
}

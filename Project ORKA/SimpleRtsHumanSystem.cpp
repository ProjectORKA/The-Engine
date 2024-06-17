#include "SimpleRtsHumanSystem.hpp"
#include "Random.hpp"
#include "Renderer.hpp"
#include "SimpleRts.hpp"

void SimpleRtsHumanSystem::spawnHuman(const SimpleRtsSimulation& sim)
{
	scales.emplace_back(randomFloatFast(minHumanSize, maxHumanSize));
	names.emplace_back(loadedNames[randomIntFast(loadedNames.size())]);
	surNames.emplace_back(loadedSurNames[randomIntFast(loadedSurNames.size())]);
	positions.emplace_back(sim.getRandomSpawnPos(humanRadius));
	targetPositions.emplace_back(0, 0);
	directions.emplace_back(randomUnitVec2Fast());
	speed.emplace_back(randomFloatFast(humanSpeedMin, humanSpeedMax));
	states.push_back(SimpleRtsHumanState::Wandering);
	humanCount++;
}

void SimpleRtsHumanSystem::create(const SimpleRtsSimulation& sim)
{
	logDebug("Spawning Humans");
	for (int i = 0; i < count; i++) spawnHuman(sim);
	logDebug("Spawning Humans Done");
}

void SimpleRtsHumanSystem::update(const SimpleRtsSimulation& sim)
{
	for (int i = 0; i < humanCount; i++)
	{
		// Wandering
		if (states[i] == SimpleRtsHumanState::Wandering)
		{
			positions[i] += directions[i] * speed[i] * wanderingSpeed * sim.timeStep;
			loopWithinCentered(positions[i], sim.mapSize);

			if (randomFloatFast() < 0.01f) states[i] = SimpleRtsHumanState::Idle; // not consistent with time
		}

		// Idle
		if (states[i] == SimpleRtsHumanState::Idle)
		{
			if (randomFloatFast() < 0.01f) states[i] = SimpleRtsHumanState::Wandering; // not consistent with time
			if (randomFloatFast() < 0.02f) directions[i] = randomUnitVec2Fast();
		}
	}
}

void SimpleRtsHumanSystem::destroy(const SimpleRtsSimulation& sim) {}

bool SimpleRtsHumanSystem::doesCollide(const Vec2 position, const Float radius) const
{
	return circleCollidingWithCircles(position, radius, positions, humanRadius);
}

void SimpleRtsHumanSystem::render(Renderer& renderer) const
{
	renderer.renderMeshInstanced("Human", positions, directions, scales);
}

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
	hunger.push_back(0.0);
	targetId.push_back(0);
	humanCount++;
}

void SimpleRtsHumanSystem::create(const SimpleRtsSimulation& sim)
{
	loadedNames    = loadStringVector("Data/Text/simpleRTSHumanMaleNames.txt");
	loadedSurNames = loadStringVector("Data/Text/simpleRTSHumanSurnames.txt");

	logDebug("Spawning Humans");
	for (int i = 0; i < count; i++) spawnHuman(sim);
	logDebug("Spawning Humans Done");
}

void SimpleRtsHumanSystem::update(SimpleRtsSimulation& sim)
{
	timer.start();
	for (int i = 0; i < humanCount; i++)
	{
		if (states[i] != SimpleRtsHumanState::Dead)
		{
			hunger[i] += sim.timeStep() * 5.0;
			if (hunger[i] > 100.0) die(i);

			// Wandering
			if (states[i] == SimpleRtsHumanState::Wandering)
			{
				positions[i] += directions[i] * speed[i] * wanderingWalkingSpeedMultiplier * sim.timeStep();
				loopWithinCentered(positions[i], sim.mapSize);

				if (hunger[i] > 50.0)
				{
					states[i] = SimpleRtsHumanState::SearchFood;
					continue;
				}
				if (randomFloatFast() < 0.01f) states[i] = SimpleRtsHumanState::Idle; // not consistent with time
			}

			// Idle
			if (states[i] == SimpleRtsHumanState::Idle)
			{
				if (hunger[i] > 50.0)
				{
					states[i] = SimpleRtsHumanState::SearchFood;
					continue;
				}

				if (randomFloatFast() < 0.01f) states[i] = SimpleRtsHumanState::Wandering; // not consistent with time
				if (randomFloatFast() < 0.02f) directions[i] = randomUnitVec2Fast();
			}

			if (states[i] == SimpleRtsHumanState::SearchFood)
			{
				Index berryBushId  = idOfClosestPointInLoopingSpace(positions[i], sim.berryBushSystem.positions, sim.mapSize);
				targetId[i]        = berryBushId;
				targetPositions[i] = sim.berryBushSystem.positions[berryBushId];
				directions[i]      = normalize(deltaInLoopingSpace(positions[i], targetPositions[i], sim.mapSize));
				states[i]          = SimpleRtsHumanState::MoveToFood;
			}

			if (states[i] == SimpleRtsHumanState::MoveToFood)
			{
				Vec2  delta        = deltaInLoopingSpace(positions[i], targetPositions[i], sim.mapSize);
				Float moveDistance = speed[i] * hungerWalkingSpeedMultiplier * sim.timeStep();
				Vec2  step         = normalize(delta) * moveDistance;
				directions[i]      = normalize(delta);
				if (length(step) >= length(delta))
				{
					step = delta;
					sim.berryBushSystem.eat(targetId[i], sim);
					hunger[i] = 0.0;
					states[i] = SimpleRtsHumanState::Idle;
				}
				positions[i] += step;
				loopWithinCentered(positions[i], sim.mapSize);
			}
		}
	}
	timer.stop();
}

void SimpleRtsHumanSystem::destroy(const SimpleRtsSimulation& sim) {}

void SimpleRtsHumanSystem::die(const Index i)
{
	scales[i] = 0.0;
	println(names[i] + " is fucking ded");
	states[i] = SimpleRtsHumanState::Dead;
}

bool SimpleRtsHumanSystem::doesCollide(const Vec2 position, const Float radius) const
{
	return circleCollidingWithCircles(position, radius, positions, humanRadius);
}

void SimpleRtsHumanSystem::render(Renderer& renderer) const
{
	renderer.renderMeshInstanced("Human", positions, directions, scales);
}

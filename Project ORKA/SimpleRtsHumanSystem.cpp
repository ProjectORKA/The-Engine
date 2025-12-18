#include "SimpleRtsHumanSystem.hpp"

#include "FileSystem.hpp"
#include "Random.hpp"
#include "Renderer.hpp"
#include "SimpleRts.hpp"

void SimpleRtsHumanSystem::spawnHuman(const SimpleRtsSimulation& sim)
{
	scales.emplace_back(randomFloatFast(minHumanSize, maxHumanSize));
	names.emplace_back(loadedNames[randomIntFast(loadedNames.size())]);
	surNames.emplace_back(loadedSurNames[randomIntFast(loadedSurNames.size())]);
	positions.emplace_back(sim.getRandomSpawnPos());
	targetPositions.emplace_back(0, 0);
	directions.emplace_back(randomUnitVec2Fast());
	speed.emplace_back(randomFloatFast(humanSpeedMin, humanSpeedMax));
	states.push_back(SimpleRtsHumanState::Wandering);
	const Float energyRequirement = randomFloatFast(2000,3000);
	dailyEnergyRequirement.push_back(energyRequirement);
	storedEnergy.push_back(randomFloatFast(0,energyRequirement));
	targetId.push_back(0);
	humanCount++;
}

void SimpleRtsHumanSystem::create(const SimpleRtsSimulation& sim)
{
	name           = "HumanSystem";
	loadedNames    = loadStringVector("Data/Text/simpleRTSHumanMaleNames.txt");
	loadedSurNames = loadStringVector("Data/Text/simpleRTSHumanSurnames.txt");

	names.reserve(count);
	speed.reserve(count);
	scales.reserve(count);
	states.reserve(count);
	dailyEnergyRequirement.reserve(count);
	storedEnergy.reserve(count);
	surNames.reserve(count);
	targetId.reserve(count);
	positions.reserve(count);
	directions.reserve(count);
	targetPositions.reserve(count);

	if (debugSimpleRts) logDebug("Spawning " + std::to_string(count) + " humans");
	for (int i = 0; i < count; i++) spawnHuman(sim);
	if (debugSimpleRts) logDebug("Spawned humans");
}

void SimpleRtsHumanSystem::update(SimpleRtsSimulation& sim)
{
	timer.start();
	for (int i = 0; i < humanCount; i++)
	{
		if (states[i] != SimpleRtsHumanState::Dead)
		{
			storedEnergy[i] -= sim.timeStep() * dailyEnergyRequirement[i] / 86400.0f;

			if (storedEnergy[i] <= 0.0f)
			{
				scales[i] = 0.0;
				logDebug(names[i] + " is dead");
				states[i] = SimpleRtsHumanState::Dead;
			}

			if (states[i] == SimpleRtsHumanState::Wandering)
			{
				positions[i] += directions[i] * speed[i] * wanderingWalkingSpeedMultiplier * sim.timeStep();
				loopWithinCentered(positions[i], sim.mapSize);
				if (storedEnergy[i] < 0.5f * dailyEnergyRequirement[i])
				{
					states[i] = SimpleRtsHumanState::SearchFood;
					continue;
				}
				if (randomFloatFast() < 0.01f) states[i] = SimpleRtsHumanState::Idle; // not consistent with time
			}

			// Idle
			if (states[i] == SimpleRtsHumanState::Idle)
			{
				if (storedEnergy[i] < 0.5f * dailyEnergyRequirement[i])
				{
					states[i] = SimpleRtsHumanState::SearchFood;
					continue;
				}

				if (randomFloatFast() < 0.01f) states[i] = SimpleRtsHumanState::Wandering; // not consistent with time
				if (randomFloatFast() < 0.02f) directions[i] = randomUnitVec2Fast();
			}

			if (states[i] == SimpleRtsHumanState::SearchFood)
			{
				if (const Vec2 delta = deltaInLoopingSpace(positions[i], targetPositions[i], sim.mapSize); delta != Vec2(0)) directions[i] = normalize(delta);
				const Index berryBushId = idOfClosestPointInLoopingSpace(positions[i], sim.berryBushSystem.positions, sim.mapSize);
				targetId[i]             = berryBushId;
				targetPositions[i]      = sim.berryBushSystem.positions[berryBushId];
				states[i]               = SimpleRtsHumanState::MoveToFood;
			}

			if (states[i] == SimpleRtsHumanState::MoveToFood)
			{
				// Calculate delta in looping space and its length
				const Vec2  delta = deltaInLoopingSpace(positions[i], targetPositions[i], sim.mapSize);
				const Float dist  = length(delta);

				if (delta == Vec2(0) || dist == 0.0)
				{
					states[i] = SimpleRtsHumanState::EatFood;
					return;
				}
				const Float moveDistance = speed[i] * hungerWalkingSpeedMultiplier * sim.timeStep();
				directions[i] = delta / dist;

				if (moveDistance >= dist)
				{
					positions[i] += delta;
					states[i] = SimpleRtsHumanState::EatFood;
				}
				else positions[i] += directions[i] * moveDistance;
				loopWithinCentered(positions[i], sim.mapSize);
			}

			if (states[i] == SimpleRtsHumanState::EatFood)
			{
				sim.berryBushSystem.eat(targetId[i], sim);
				storedEnergy[i] += 1294.0f; // kcal of a berry bush
				states[i] = SimpleRtsHumanState::Idle;
			}
		}
	}
	timer.stop();
}

void SimpleRtsHumanSystem::destroy(const SimpleRtsSimulation& sim) {}

Bool SimpleRtsHumanSystem::doesCollide(const Vec2 position, const Float radius) const
{
	return false; //circleCollidingWithCircles(position, radius, positions, humanRadius);
}

void SimpleRtsHumanSystem::render(Renderer& renderer)
{
	renderer.renderMeshInstanced("Human", positions, directions, scales);
}

#pragma once

#include "Basics.hpp"
#include "Renderer.hpp"
#include "SimpleRtsSystem.hpp"

struct Food
{
	Float volume = 0;
	Float kcal   = 0;
};

struct Renderer;
struct SimpleRtsSimulation;

enum class SimpleRtsHumanState
{
	Dead,
	Idle,
	Wandering,
	SearchFood,
	MoveToFood,
	EatFood,
};

struct SimpleRtsHumanSystem : SimpleRtsSystem
{
	void               render(Renderer& renderer) override;
	void               update(SimpleRtsSimulation& sim) override;
	void               spawnHuman(const SimpleRtsSimulation& sim);
	[[nodiscard]] Bool doesCollide(Vec2 position, Float radius) const;
	void               create(const SimpleRtsSimulation& sim) override;
	void               destroy(const SimpleRtsSimulation& sim) override;

private:
	Timer       timer;
	const Float humanRadius                     = 0.5f;
	const UInt  count                           = 10000;
	const Float minHumanSize                    = 0.8f;
	const Float maxHumanSize                    = 1.2f;
	const Float humanSpeedMin                   = 2.0;
	const Float humanSpeedMax                   = 4.0;
	const Float hungerWalkingSpeedMultiplier    = 1.5;
	const Float wanderingWalkingSpeedMultiplier = 1.0;

	UInt humanCount = 0;

	Vector<String> loadedNames;
	Vector<String> loadedSurNames;

	Vector<String>              names;
	Vector<Float>               speed;
	Vector<SimpleRtsHumanState> states;

	Vector<Float> storedEnergy;
	Vector<Float> dailyEnergyRequirement;

	Vector<String> surNames;
	Vector<Index>  targetId;
	Vec2Vector     targetPositions;
	Vec3Vector     renderPositions;
	Vector<Float>  scales;
	Vec2Vector     positions;
	Vec2Vector     directions;
};

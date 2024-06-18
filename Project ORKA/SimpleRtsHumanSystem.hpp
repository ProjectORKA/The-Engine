#pragma once

#include "Basics.hpp"
#include "FileSystem.hpp"
#include "Renderer.hpp"
#include "SimpleRtsSystem.hpp"

struct Renderer;
struct SimpleRtsSimulation;

enum class SimpleRtsHumanState
{
	Dead,
	Idle,
	Wandering,
	SearchFood,
	MoveToFood,
};

struct SimpleRtsHumanSystem : SimpleRtsSystem
{
	Timer timer;
	const Float humanRadius                     = 0.5f;
	const UInt  count                           = 1000;
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
	Vector<Float>               scales;
	Vector<SimpleRtsHumanState> states;
	Vector<Float>               hunger;
	Vector<String>              surNames;
	Vector<Vec2>                positions;
	Vector<Vec2>                directions;
	//targets
	Vector<Index>				targetId;
	Vector<Vec2>                targetPositions;

	void render(Renderer& renderer) const override;
	void spawnHuman(const SimpleRtsSimulation& sim);
	bool doesCollide(Vec2 position, Float radius) const;
	void create(const SimpleRtsSimulation& sim) override;
	void update(SimpleRtsSimulation& sim) override;
	void destroy(const SimpleRtsSimulation& sim) override;

	void die(Index i);
};

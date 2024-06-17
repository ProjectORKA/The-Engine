#pragma once

#include "Basics.hpp"
#include "FileSystem.hpp"
#include "Renderer.hpp"
#include "SimpleRtsSystem.hpp"

struct Renderer;
struct SimpleRtsSimulation;

enum class SimpleRtsHumanState
{
	Idle,
	Wandering
};

struct SimpleRtsHumanSystem : SimpleRtsSystem
{
	const Float humanRadius    = 0.5f;
	const UInt  count          = 1000;
	const Float minHumanSize   = 0.8f;
	const Float maxHumanSize   = 1.2f;
	const Float humanSpeedMin  = 2.0;
	const Float humanSpeedMax  = 4.0;
	const Float wanderingSpeed = 1.0;

	UInt humanCount = 0;

	const Vector<String> loadedNames    = loadStringVector("Data/Text/simpleRTSHumanMaleNames.txt");
	const Vector<String> loadedSurNames = loadStringVector("Data/Text/simpleRTSHumanSurnames.txt");

	Vector<String>              names;
	Vector<Float>               speed;
	Vector<Float>               scales;
	Vector<SimpleRtsHumanState> states;
	Vector<String>              surNames;
	Vector<Vec2>                positions;
	Vector<Vec2>                directions;
	Vector<Vec2>                targetPositions;

	void render(Renderer& renderer) const override;
	void spawnHuman(const SimpleRtsSimulation& sim);
	bool doesCollide(Vec2 position, Float radius) const;
	void create(const SimpleRtsSimulation& sim) override;
	void update(const SimpleRtsSimulation& sim) override;
	void destroy(const SimpleRtsSimulation& sim) override;
};

#pragma once

#include "OrkaTypes.hpp"

struct AgentValue;

struct SimpleRtsWorldSettings
{
	static constexpr Int formatVersion = 1;

	Float mapSize = 2000.0f;
	Bool enableWater = true;
	Bool enableElevation = true;
	Int terrainPointCount = 200000;

	Int humans = 10000;
	Int rabbits = 10000;
	Int wolves = 10000;
	Int trees = 100;
	Int bushes = 10000;
	Int berryBushes = 10000;
	Int bodies = 1000;

	[[nodiscard]] static SimpleRtsWorldSettings defaults();
	[[nodiscard]] static SimpleRtsWorldSettings smallTestWorld();
	[[nodiscard]] static SimpleRtsWorldSettings flatArenaWorld();
	[[nodiscard]] static SimpleRtsWorldSettings fullSimulationWorld();

	[[nodiscard]] AgentValue toJson() const;
	[[nodiscard]] static SimpleRtsWorldSettings fromJson(const AgentValue& value, String* errorOut = nullptr);
};

#pragma once

#include "Game.hpp"
#include "Player.hpp"
#include "PerformanceGraph.hpp"
#include "SimpleRtsBushSystem.hpp"
#include "SimpleRtsBodySystem.hpp"
#include "SimpleRTSTreeSystem.hpp"
#include "SimpleRtsWolfSystem.hpp"
#include "SimpleRtsHumanSystem.hpp"
#include "SimpleRtsRabbitSystem.hpp"
#include "SimpleRtsTerrainSystem.hpp"
#include "SimpleRtsQuadtreeSystem.hpp"
#include "SimpleRtsBerryBushSystem.hpp"
#include "SimpleRtsWorldSettings.hpp"

extern Bool debugSimpleRts;

struct SimpleRtsSimulation final : GameSimulation
{
	SimpleRtsWorldSettings worldSettings;

	ULL    frame     = 0;
	Double totalTime = 0.0f;
	Float  timeScale = 1.0f;
	Float  frameRate = 60.0f;
	Bool   paused    = false;

	SimpleRtsBodySystem      bodySystem;
	SimpleRtsBushSystem      bushSystem;
	SimpleRtsTreeSystem      treeSystem;
	SimpleRtsHumanSystem     humanSystem;
	SimpleRtsRabbitSystem    rabbitSystem;
	SimpleRtsTerrainSystem   terrainSystem;
	SimpleRtsQuadtreeSystem  quadtreeSystem;
	SimpleRtsBerryBushSystem berryBushSystem;
	SimpleRtsWolfSystem      wolfSystem;

	Vector<SimpleRtsSystem*> systems;

	[[nodiscard]] Float timeStep() const;
	[[nodiscard]] Vec3 getRandomSpawnPos() const;
	[[nodiscard]] Vec3 getRandomSpawnPos(Float radius) const;
	[[nodiscard]] Bool doesCollide(Vec2 pos, Float radius) const;

	void reset();
	void create() override;
	void destroy() override;
	void waitUntilLoaded() const;
	void render(Renderer& r) const;
	void update(Float delta) override;
	Float getTerrainHeight(const Vec2 vec) const;
};

struct SimpleRts
{
	SimpleRtsSimulation sim;

	void run();
};

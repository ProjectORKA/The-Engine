#pragma once

#include "Game.hpp"
#include "Player.hpp"
#include "PerformanceGraph.hpp"
#include "SimpleRtsBushSystem.hpp"
#include "SimpleRtsBodySystem.hpp"
#include "SimpleRTSTreeSystem.hpp"
#include "SimpleRtsHumanSystem.hpp"
#include "SimpleRtsRabbitSystem.hpp"
#include "SimpleRtsTerrainSystem.hpp"
#include "SimpleRtsQuadtreeSystem.hpp"
#include "SimpleRtsBerryBushSystem.hpp"

struct SimpleRtsSimulation final : GameSimulation
{
	const Float mapSize = 1000.0f;

	Float time      = 0.0f;
	Float frameRate = 60.0f;
	Bool  paused    = false;
	Float timeScale = 1.0f;

	SimpleRtsQuadtreeSystem quadtreeSystem;

	SimpleRtsBodySystem      bodySystem;
	SimpleRtsBushSystem      bushSystem;
	SimpleRtsTreeSystem      treeSystem;
	SimpleRtsHumanSystem     humanSystem;
	SimpleRtsRabbitSystem    rabbitSystem;
	SimpleRtsTerrainSystem   terrainSystem;
	SimpleRtsBerryBushSystem berryBushSystem;

	Vector<SimpleRtsSystem*> systems;

	[[nodiscard]] Float timeStep() const;
	[[nodiscard]] Vec2  getRandomSpawnPos(Float radius) const;
	[[nodiscard]] Bool  doesCollide(Vec2 pos, Float radius) const;

	void create() override;
	void destroy() override;
	void update(Float delta) override;
	void render(Renderer& renderer) const;
};

struct SimpleRtsRenderer final : GameRenderer
{
	Mutex                mutex;
	DebugPlayer          player;
	PerformanceGraph     performanceGraph;
	Bool                 wireframeMode = false;
	SimpleRtsSimulation* sim           = nullptr;

	//input
	InputEvent enter           = InputEvent(InputType::Mouse, LMB, true);
	InputEvent exit            = InputEvent(InputType::Mouse, RMB, false);
	InputEvent pause           = InputEvent(InputType::KeyBoard, P, true);
	InputEvent toggleWireframe = InputEvent(InputType::KeyBoard, F, false);

	void update(Window& window) override;
	void create(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void render(Window& window, TiledRectangle area) override;
	void inputEvent(Window& window, InputEvent input) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct SimpleRts
{
	UserInterface       ui;
	SimpleRtsSimulation sim;
	Window              window;
	SimpleRtsRenderer   renderer;

	void run();
};

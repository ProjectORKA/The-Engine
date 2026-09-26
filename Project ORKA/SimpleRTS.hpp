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
#include "SimpleRtsSettingsPanel.hpp"

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

struct SimpleRtsRenderer final : GameRenderer
{
	SimpleRtsRenderer();

	Mutex                mutex;
	DebugPlayer          player;
	PerformanceGraph     performanceGraph;
	Bool                 wireframeMode = false;
	SimpleRtsSimulation* sim           = nullptr;
	GPUMesh terrainMesh;
	GPUMesh oceanMesh;
	SimpleRtsSettingsPanel settingsPanel;
	Window* owningWindow = nullptr;

	//input
	InputEvent enter           = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent exit            = InputEvent(InputType::Mouse, Rmb, false);
	InputEvent pause           = InputEvent(InputType::KeyBoard, P, true);
	InputEvent toggleWireframe = InputEvent(InputType::KeyBoard, F, false);
	InputEvent reset           = InputEvent(InputType::KeyBoard, R, false);
	InputEvent toggleSetup     = InputEvent(InputType::KeyBoard, F2, false);

	void startWorld();
	void restartWorld();
	void unloadWorldGpu();

	void update(Window& window) override;
	void create(Window& window) override;
	void render(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void renderScene(Window& window) override;
	void inputEvent(Window& window, InputEvent& input) override;
	void renderInteractive(Window& window) override;
};

struct SimpleRts
{
	SimpleRtsSimulation sim;
	SimpleRtsRenderer   renderer;

	void run();
};

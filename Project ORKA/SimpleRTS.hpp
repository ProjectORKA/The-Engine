#pragma once

#include "Game.hpp"
#include "Player.hpp"
#include "SimpleRtsBushSystem.hpp"
#include "SimpleRTSTreeSystem.hpp"
#include "SimpleRtsHumanSystem.hpp"
#include "SimpleRtsRabbitSystem.hpp"
#include "SimpleRtsTerrainSystem.hpp"

struct SimpleRtsSimulation final : GameSimulation
{
	const Int   frameRate  = 60;
	const Float timeScale  = 100.0f;
	const Float mapSize    = 1000.0f;
	const Float dimensions = mapSize * 2.0f;
	const Float timeStep   = timeScale / frameRate;

	Float time = 0.0f;

	SimpleRtsBushSystem    bushSystem;
	SimpleRtsTreeSystem    treeSystem;
	SimpleRtsHumanSystem   humanSystem;
	SimpleRtsRabbitSystem  rabbitSystem;
	SimpleRtsTerrainSystem terrainSystem;

	[[nodiscard]] Vec2 getRandomSpawnPos(Float radius) const;
	[[nodiscard]] Bool doesCollide(Vec2 pos, Float radius) const;

	void create() override;
	void destroy() override;
	void render(Renderer& renderer) const;
	void update(Float delta) override;
};

struct SimpleRtsRenderer final : GameRenderer
{
	Mutex                mutex;
	DebugPlayer          player;
	Bool                 wireframeMode = false;
	SimpleRtsSimulation* sim           = nullptr;

	//input
	InputEvent exit            = InputEvent(InputType::Mouse, RMB, false);
	InputEvent enter           = InputEvent(InputType::Mouse, LMB, true);
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

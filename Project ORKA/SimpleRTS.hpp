#pragma once

#include "Game.hpp"
#include "Random.hpp"
#include "Player.hpp"
#include "SimpleRTSTerrain.hpp"

#define SIMPLERTS_MAPSIZE 100

struct SimpleRTSSimulation;

Vec2 randomSpawnPosition();

enum class SimpleRTSItem
{
	Nothing,
	Wood,
	Food
};

struct RabbitSimulation
{
	Vector<Float> rabbitAge;
	Vector<Vec2>  rabbitPosition;
	Vector<Vec2>  rabbitVelocity;
	UInt          rabbitCount  = 0;
	const Float   rabbitSpeed  = 8;
	const Float   rabbitJitter = 10;

	void  spawnRabbit();
	void  updateRabbits();
	void  killRabbit(Index id);
	Index getClosestRabbit(Vec2 pos);
};

struct HouseSimulation
{
	Vector<UInt> houseClanId;
	Vector<Vec2> housePosition;
	UInt         houseCount = 0;
};

struct BushSimulation
{
	Vector<Vec2> bushPosition;
	Vector<Vec2> bushDirection;
	Vector<Vec2> berryBushPosition;
	Vector<Vec2> berryBushDirection;
};

struct TreeStumps
{
	Vector<Vec2> treeStumpPosition;
	UInt         lastTreeStump  = 0;
	UInt         treeStumpCount = 0;
};

struct SimpleRTSSimulation final : GameSimulation
{
	Float time       = 0.0f;
	Float mapSize    = 100.0f;
	Float timeStep   = 1.0f / 144.0f;
	Float dimensions = mapSize * 2.0f;

	// BushSimulation bushSimulation;
	// SimpleRTSTreeSystem treeSystem;
	// HouseSimulation houseSimulation;
	// RabbitSimulation rabbitSimulation;
	SimpleRTSTerrainSystem terrainSystem;
	// SimpleRTSHumanSystem humanitySimulation;

	void destroy() override;
	void update(Float delta) override;
	void create() override;
	void render(Renderer& renderer) const;
};

struct SimpleRTSRenderer final : GameRenderer
{
	Mutex                           mutex;
	DebugPlayer                     player;
	SimpleRTSSimulation*            sim           = nullptr;
	Bool                            wireframeMode = false;
	SimpleRTSTerrainRenderingSystem terrainRenderingSystem;

	//input
	InputEvent exit            = InputEvent(InputType::Mouse, RMB, false);
	InputEvent enter           = InputEvent(InputType::Mouse, LMB, true);
	InputEvent toggleWireframe = InputEvent(InputType::KeyBoard, F, false);

	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(Window& window) override;
	void render(Window& window, TiledRectangle area) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct SimpleRTS
{
	UserInterface       ui;
	SimpleRTSSimulation sim;
	Window              window;
	SimpleRTSRenderer   renderer;

	void run()
	{
		ui.create();
		sim.start();
		renderer.connect(sim);
		ui.window("Simple RTS", Area(1920, 1080), true, true, WindowState::Windowed, renderer);
		ui.run();
		sim.stop();
	}
};

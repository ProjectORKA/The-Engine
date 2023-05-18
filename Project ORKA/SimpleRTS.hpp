#pragma once

#include "Game.hpp"
#include "Random.hpp"
#include "Math.hpp"
#include "Player.hpp"
#include "Heightmap.hpp"
#include "SimpleRTSHuman.hpp"
#include "SimpleRTSTerrain.hpp"
#include "ResourceManager.hpp"
#include "SimpleRTSTreeSystem.hpp"

#define SIMPLERTS_MAPSIZE 100

struct SimpleRTSSimulation;

Vec2 randomSpawnPosition();

enum class SimpleRTSItem {
	Nothing,
	Wood,
	Food
};

struct RabbitSimulation {
	const Float rabbitJitter = 10;
	const Float rabbitSpeed = 8;

	UInt rabbitCount = 0;
	Vector<Float> rabbitAge;
	Vector<Vec2> rabbitPosition;
	Vector<Vec2> rabbitVelocity;

	void spawnRabbit();
	void updateRabbits();
	void killRabbit(Index id);
	Index getClosestRabbit(Vec2 pos);
};

struct HouseSimulation {
	UInt houseCount = 0;
	Vector<Vec2> housePosition;
	Vector<UInt> houseClanID;
};

struct BushSimulation {
	//bush
	Vector<Vec2> bushPosition;
	Vector<Vec2> bushDirection;

	//berry bush
	Vector<Vec2> berryBushPosition;
	Vector<Vec2> berryBushDirection;
};

struct TreeStumps {
	UInt lastTreeStump = 0;
	UInt treeStumpCount = 0;
	Vector<Vec2> treeStumpPosition;
};


struct SimpleRTSSimulation : public GameSimulation {
	Float time = 0;
	Float timestep = 1.0f / 144.0f;
	Float mapSize = 100;
	Float dimensions = mapSize * 2;

	//BushSimulation bushSimulation;
	//HouseSimulation houseSimulation;
	//RabbitSimulation rabbitSimulation;
	//SimpleRTSTreeSystem treeSystem;
	//SimpleRTSHumanSystem humanitySimulation;

	SimpleRTSTerrainSystem terrainSystem;

	SimpleRTSSimulation();
	void update(Float timestep) override;
	void render(ResourceManager& resourceManager, Renderer& renderer);
};

struct SimpleRTSRenderer : public GameRenderer {
	Mutex mutex;
	Bool wireframeMode = false;

	DebugPlayer player;
	SimpleRTSSimulation* sim = nullptr;
	SimpleRTSTerrainRenderingSystem terrainRenderingSystem;
	//Input
	InputEvent exit = InputEvent(InputType::Mouse, RMB, false);
	InputEvent enter = InputEvent(InputType::Mouse, LMB, true);
	InputEvent toggleWireframe = InputEvent(InputType::KeyBoard, F, false);

	void update(Window& window) override;
	void create(ResourceManager& resourceManager, Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct SimpleRTS {
	ResourceManager resourceManager;
	UserInterface ui;
	SimpleRTSSimulation sim;
	SimpleRTSRenderer renderer;

	void run() {
		renderer.sim = &sim;
		ui.window("Simple RTS", Area(1920, 1080), true, WindowState::windowed, renderer, resourceManager);
		ui.run();
	}
};

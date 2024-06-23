#pragma once

#include "Game.hpp"
#include "Player.hpp"
#include "SimpleRtsBushSystem.hpp"
#include "SimpleRTSTreeSystem.hpp"
#include "SimpleRtsHumanSystem.hpp"
#include "SimpleRtsRabbitSystem.hpp"
#include "SimpleRtsTerrainSystem.hpp"
#include "SimpleRtsBerryBushSystem.hpp"
#include "SimpleRtsBodySystem.hpp"



//struct SimpleRtsOctreeNode
//{
//	Index treeCount = 0;
//	Index parentNode = 0;
//	Index cnet = 0;
//	Index cnwt = 0;
//	Index cset = 0;
//	Index cswt = 0;
//	Index cneb = 0;
//	Index cnwb = 0;
//	Index cseb = 0;
//	Index cswb = 0;
//	Index nn = 0;
//	Index ne = 0;
//	Index ns = 0;
//	Index nw = 0;
//	Index level = 0;
//};
//
//struct SimpleRTSOctree
//{
//	Vector<Vector<SimpleRtsOctreeNode>> levels;
//	Vector<Float> sizes;
//
//	void create()
//	{
//		levels.emplace_back().emplace_back();
//	}
//
//	void destroy()
//	{
//		levels.clear();
//	}
//};

struct SimpleRtsSimulation final : GameSimulation
{
	const Float dimensions = mapSize * 2.0f;
	const Float mapSize    = 1000.0f;

	Float time      = 0.0f;
	Float frameRate = 60.0f;
	Bool  paused    = false;
	Float timeScale = 1.0f;

	//SimpleRTSOctree octree;

	SimpleRtsBodySystem bodySystem;
	SimpleRtsBushSystem bushSystem;
	SimpleRtsTreeSystem treeSystem;
	SimpleRtsHumanSystem humanSystem;
	SimpleRtsRabbitSystem rabbitSystem;
	SimpleRtsTerrainSystem terrainSystem;
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

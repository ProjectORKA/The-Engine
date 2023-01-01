
#pragma once

#include "Game.hpp"
#include "Basics.hpp"
#include "KeyMap.hpp"
#include "Player.hpp"

//#include "Renderer.hpp"
struct Renderer;
struct Window;

#include "DynameshTerrain.hpp"
#include "GeometryClipmapTerrain.hpp"

struct SandboxSimulation : public GameSimulation {

	Vec3 location = Vec3(0);

	SandboxSimulation();
	~SandboxSimulation();
	void update(Float timestep) override;
};

struct Sandbox : public GameRenderer {
	
	Mutex mutex;
	ClipMap system;
	DebugPlayer player;
	Float mouseSensitivity = 0.0015f;
	SandboxSimulation * simulation = nullptr;

	Sandbox(SandboxSimulation& sim);
	void create(Window& window) override;
	void update(Window& window) override;
	void render(Engine & engine, Window& window, TiledRectangle area) override;
};
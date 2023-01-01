#pragma once

#include "Game.hpp"
#include "Random.hpp"
#include "Engine.hpp"

const Float particleSize = 30;

struct PhysicsParticle {
	Vec2 position = randomVec2(0,1000);
	Vec2 velocity = randomVec2(-100, 100);
	Bool isColliding = false;

	void collisionCheck();
	void update(Float delta);
	void render(Engine& engine, Window& window);
};

struct PhysicsPlaygroundSimulation : public GameSimulation {
	Vector<PhysicsParticle> particles;

	PhysicsPlaygroundSimulation();
	void update(Float delta) override;
};

struct PhysicsPlayGroundRenderer : public GameRenderer {
	PhysicsPlaygroundSimulation* sim = nullptr;

	PhysicsPlayGroundRenderer(PhysicsPlaygroundSimulation& simulation);
	void update(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override {};
	void render(Engine& engine, Window& window, TiledRectangle area) override;
	void renderInteractive(Engine & engine, Window& window, TiledRectangle area) override {};
};
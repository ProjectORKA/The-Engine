#pragma once

#include "Game.hpp"
#include "Random.hpp"

const Float particleSize = 30;

struct PhysicsParticle {
	Vec2 position = randomVec2(0,1000);
	Vec2 velocity = randomVec2(-100, 100);
	Bool isColliding = false;

	void collisionCheck();
	void update(Float delta);
	void render(Window& window);
};

struct PhysicsPlaygroundSimulation : public GameSimulation {
	Vector<PhysicsParticle> particles;

	PhysicsPlaygroundSimulation();
	void update(Float delta) override;
};

struct PhysicsPlayGroundRenderer : public GameRenderer {
	PhysicsPlaygroundSimulation* sim = nullptr;

	PhysicsPlayGroundRenderer(PhysicsPlaygroundSimulation& simulation);
	virtual void update(Window& window);
	virtual void render(Window& window, TiledRectangle area);
	virtual void inputEvent(Window& window, InputEvent input) {};
	virtual void renderInteractive(Window& window, TiledRectangle area) {};
};
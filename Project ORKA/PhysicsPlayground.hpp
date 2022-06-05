#pragma once

#include "Game.hpp"
#include "Random.hpp"
#include "Physics.hpp"

struct PhysicsParticle {
	Vec2 position = randomVec2(500,1500);
	Vec2 velocity = Vec2(0);

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
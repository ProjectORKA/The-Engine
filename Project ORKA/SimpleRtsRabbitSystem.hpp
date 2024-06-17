#pragma once
#include "Basics.hpp"
#include "SimpleRtsSystem.hpp"

struct SimpleRtsSimulation;
struct Renderer;

struct SimpleRtsRabbitSystem : SimpleRtsSystem
{
	const Float rabbitRadius = 0.2f;

	UInt        rabbitCount  = 0;
	const Float rabbitSpeed  = 8;
	const Float rabbitJitter = 10;

	Vector<Float> age;
	Vector<Vec2>  positions;
	Vector<Vec2>  velocity;

	void render(Renderer& renderer) const override;
	void spawnRabbit(const SimpleRtsSimulation& sim);
	void create(const SimpleRtsSimulation& sim) override {}
	void update(const SimpleRtsSimulation& sim) override {}
	void destroy(const SimpleRtsSimulation& sim) override {}
	bool doesCollide(Vec2 position, Float radius) const;
};

#pragma once
#include "Basics.hpp"
#include "SimpleRtsSystem.hpp"

enum RabbitState
{
	Standing,
	Hopping,
	Running
};

struct SimpleRtsRabbitSystem : SimpleRtsSystem
{
	const Float hopSpeed      = 0.5f;
	const Float rabbitRadius  = 0.2f;
	const Float rabbitMaxAge  = 20.0f * 60.0f;
	const Float rabbitMinSize = 0.2f;
	const Float rabbitMaxSize = 1.0f;
	const Float minSpeed      = 0.1f;
	const Float maxSpeed      = 8.0f;
	const Float jitter        = 0.1f;
	const Int   count         = 10000;

	Vector<Float>       sizes;
	Vector<Float>       ages;
	Vec2Vector          positions;
	Vec3Vector          renderPositions;
	Vec2Vector          directions;
	Vector<Float>       speeds;
	Vector<RabbitState> states;
	Vector<Double>      waitTimes;

	void killRabbit(Int id);
	void render(Renderer& renderer) override;
	void update(SimpleRtsSimulation& sim) override;
	void spawnEntity(const SimpleRtsSimulation& sim);
	void create(const SimpleRtsSimulation& sim) override;
	void destroy(const SimpleRtsSimulation& sim) override;

	[[nodiscard]] Float sizeFromAge(Float age) const;
	[[nodiscard]] Float speedFromAge(Float age) const;
	[[nodiscard]] Bool  doesCollide(Vec2 position, Float radius) const;
};

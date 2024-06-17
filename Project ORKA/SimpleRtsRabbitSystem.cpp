#include "SimpleRtsRabbitSystem.hpp"
#include "Renderer.hpp"
#include "SimpleRts.hpp"

void SimpleRtsRabbitSystem::spawnRabbit(const SimpleRtsSimulation& sim)
{
	age.push_back(0.0);
	positions.push_back(sim.getRandomSpawnPos(rabbitRadius));
	velocity.push_back(Vec2(0.0f, 0.0f));
}

bool SimpleRtsRabbitSystem::doesCollide(const Vec2 position, const Float radius) const
{
	return circleCollidingWithCircles(position, radius, positions, rabbitRadius);
}

void SimpleRtsRabbitSystem::render(Renderer& renderer) const
{
	renderer.renderMeshInstanced("Rabbit", positions);
}

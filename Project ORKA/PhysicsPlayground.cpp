#include "PhysicsPlayground.hpp"
#include "PhysicsSystem.hpp"

void PhysicsPlaygroundSimulation::destroy() {}

void PhysicsPlayGroundRenderer::update(Window& window) {}

void PhysicsPlayGroundRenderer::destroy(Window& window) {}


void PhysicsPlaygroundSimulation::update(const Float delta)
{
	constexpr Vec2 acceleration = gravity;
	for(PhysicsParticle& p : particles)
	{
		verletPhysics(p.position, p.velocity, acceleration, delta);

		if(p.position.x < -1)
		{
			p.position.x = -1;
			p.velocity.x *= -1;
		}
		if(p.position.x > +1)
		{
			p.position.x = +1;
			p.velocity.x *= -1;
		}
		if(p.position.y < -1)
		{
			p.position.y = -1;
			p.velocity.y *= -1;
		}
		if(p.position.y > +1)
		{
			p.position.y = +1;
			p.velocity.y *= -1;
		}

		//loopWithinCentered(p.position, 1);
	}
}

void PhysicsPlayGroundRenderer::connect(GameSimulation& simulation)
{
	this->sim = static_cast<PhysicsPlaygroundSimulation*>(&simulation);
}

void PhysicsPlaygroundSimulation::create()
{
	while(particles.size() < nodeCount) particles.emplace_back();
}

void PhysicsPlayGroundRenderer::create(Window& window) {}

void PhysicsPlayGroundRenderer::inputEvent(Window& window, const InputEvent input)
{
	if(input == pause) sim->paused = !sim->paused;
}

Float PhysicsPlaygroundSimulation::distanceBetweenTwoNodes(const Index a, const Index b) const
{
	return distance(particles[a].position, particles[b].position);
}

void PhysicsPlayGroundRenderer::render(Window& window, TiledRectangle area)
{
	Renderer& renderer = window.renderer;
	renderer.clearBackground(Color(0, 0, 0, 1));
	renderer.aspectCorrectNormalizedSpace();

	// render particles
	renderer.fill(Color(1));
	renderer.useShader("color");
	Vector<Matrix> particleMatrices;
	for(PhysicsParticle& p : sim->particles)
	{
		particleMatrices.emplace_back(matrixFromPositionAndSize(p.position, Vec2(particleSize)));
	}
	renderer.circles(particleMatrices);
}

void PhysicsPlayGroundRenderer::renderInteractive(Window& window, TiledRectangle area) {}

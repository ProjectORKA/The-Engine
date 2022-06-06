
#include "PhysicsPlayground.hpp"
#include "Window.hpp"
#include "Physics.hpp"

void PhysicsParticle::update(Float delta) {
	Vec2 gravity = Vec2(0, 0);
	verletPhysics(position, velocity, gravity, delta);

	loopPointInSpace2D(position, Vec2(1000, 1000));

	collisionCheck();
}

void PhysicsParticle::collisionCheck() {

}

void PhysicsParticle::render(Window& window) {
	Renderer& renderer = window.renderer;
	renderer.circle(position, particleSize);
}

PhysicsPlayGroundRenderer::PhysicsPlayGroundRenderer(PhysicsPlaygroundSimulation& simulation) {
	sim = &simulation;
}

void PhysicsPlayGroundRenderer::update(Window& window) {

}

void PhysicsPlayGroundRenderer::render(Window& window, TiledRectangle area) {
	Renderer& renderer = window.renderer;

	renderer.clearColor(Color(0));

	renderer.screenSpace();

	renderer.fill(Color(1));
	renderer.useShader("color");

	for (PhysicsParticle& p : sim->particles) {
		p.render(window);
	}

}

void PhysicsPlaygroundSimulation::update(Float delta) {
	for (PhysicsParticle& p : particles) {
		p.update(delta);
	}
}

PhysicsPlaygroundSimulation::PhysicsPlaygroundSimulation() {
	while (particles.size() < 100) {
		particles.emplace_back();
	}
}

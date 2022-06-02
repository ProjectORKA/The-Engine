
#include "PhysicsPlayground.hpp"
#include "Window.hpp"

void PhysicsParticle::update(Float delta) {
	Vec2 gravity = Vec2(0, -10);
	verletPhysics(position, velocity, gravity, delta);

	collisionCheck();
}

void PhysicsParticle::collisionCheck() {
	if (position.y <= 0.5) {
		position.y = 0.5;
		velocity.y *= -1;
	}
}

void PhysicsParticle::render(Window& window) {
	Renderer& renderer = window.renderer;
	renderer.circle(position, 5);
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

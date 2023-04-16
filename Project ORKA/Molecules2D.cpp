
#include "Molecules2D.hpp"
#include "Renderer.hpp"

void Molecule2D::move() {
	if (numforces) {
		pos += forces / Vec2(numforces);
		forces = Vec2(0);
		numforces = 0;
	}
}
void Molecule2D::addForce(Vec2 force) {
	forces += force;
	numforces++;
}
void Molecule2D::collide(Molecule2D& p) {

	Vec2 delta = p.pos - pos;
	if (delta != Vec2(0)) {
		Float intersectionDistance = 2 - length(delta);
		if (intersectionDistance > 0.0) {
			Vec2 force = normalize(delta) * intersectionDistance;
			p.addForce(force);
			addForce(-force);
		}
	}
}
void Molecule2D::render(ResourceManager& resourceManager, Renderer& renderer) {
	renderer.circle(resourceManager, pos, 1);
}

void Molecules2D::update(Vec3 location) {
	while (molecules.size() < 2000) {
		molecules.emplaceBack();
		molecules.last().pos = randomVec2(-1,1);
	}

	for (UInt i = 0; i < molecules.size(); i++) {
		for (UInt j = i + 1; j < molecules.size(); j++) {
			molecules[i].collide(molecules[j]);
		}
	}
	
	for (Molecule2D & m : molecules) {
		m.move();
	}

	molecules.first().pos = location;
}
void Molecules2D::render(ResourceManager& resourceManager, Renderer& renderer) {
	
	renderer.useShader(resourceManager, "color");
	renderer.fill(Color(1));	
	for (Molecule2D m : molecules) {
		m.render(resourceManager, renderer);
	}
}
#pragma once

#include "Basics.hpp"
#include "Random.hpp"

struct Renderer;

struct Molecule2D {
	UInt row = random(ULL(sqrt(3000)));

	Vec2 pos = Vec2(0);
	Vec2 forces = Vec2(0);
	UShort numforces = 0;

	void move();
	void addForce(Vec2 force);
	void collide(Molecule2D& p);
	void render(Renderer& renderer);
};

struct Molecules2D {
	Vector<Molecule2D> molecules;

	void update(Vec3 location);
	void render(Renderer& renderer);
};
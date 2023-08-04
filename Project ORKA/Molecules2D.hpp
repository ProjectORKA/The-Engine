#pragma once

#include "Basics.hpp"
#include "Random.hpp"
#include "ResourceManager.hpp"

struct Renderer;

struct Molecule2D
{
	UShort numforces = 0;
	Vec2   pos       = Vec2(0);
	Vec2   forces    = Vec2(0);
	UInt   row       = random(static_cast<ULL>(sqrt(3000)));

	void move();
	void addForce(Vec2 force);
	void collide(Molecule2D& p);
	void render(ResourceManager& resourceManager, Renderer& renderer) const;
};

struct Molecules2D
{
	Vector<Molecule2D> molecules;

	void update(Vec3 location);
	void render(ResourceManager& resourceManager, Renderer& renderer) const;
};

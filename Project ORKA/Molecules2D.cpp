#include "Molecules2D.hpp"
#include "Renderer.hpp"

void Molecule2D::move()
{
	if(numforces)
	{
		pos += forces / Vec2(numforces);
		forces    = Vec2(0);
		numforces = 0;
	}
}

void Molecule2D::collide(Molecule2D& p)
{
	const Vec2 delta = p.pos - pos;
	if(delta != Vec2(0))
	{
		const Float intersectionDistance = 2 - length(delta);
		if(intersectionDistance > 0.0)
		{
			const Vec2 force = normalize(delta) * intersectionDistance;
			p.addForce(force);
			addForce(-force);
		}
	}
}

void Molecule2D::addForce(const Vec2 force)
{
	forces += force;
	numforces++;
}

void Molecules2D::update(const Vec3 location)
{
	while(molecules.size() < 2000)
	{
		molecules.emplace_back();
		molecules.back().pos = randomVec2Fast(-1.0, 1.0);
	}

	for(UInt i = 0; i < molecules.size(); i++) for(UInt j = i + 1; j < molecules.size(); j++) molecules[i].collide(molecules[j]);

	for(Molecule2D& m : molecules) m.move();

	molecules.front().pos = location;
}

void Molecule2D::render(ResourceManager& resourceManager, Renderer& renderer) const
{
	renderer.circle(resourceManager, pos, 1);
}

void Molecules2D::render(ResourceManager& resourceManager, Renderer& renderer) const
{
	renderer.useShader(resourceManager, "color");
	renderer.fill(Color(1));
	for(Molecule2D m : molecules) m.render(resourceManager, renderer);
}

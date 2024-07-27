#include "SimpleRtsQuadtreeSystem.hpp"
#include "Math.hpp"
#include "SimpleRTS.hpp"

void SimpleRtsQuadtreeSystem::render(Renderer& r) const
{
	r.wireframeCubes(quadtreeNodeMatrices);
}

void SimpleRtsQuadtreeSystem::create(SimpleRtsSimulation& sim)
{
	nodes = Vector<Vector<SimpleRtsQuadtreeNode>>(levelCount);
	nodes[0].emplace_back(sim, 0, 0, 0, -1, 0);
}

void SimpleRtsQuadtreeSystem::update(SimpleRtsSimulation& sim, const Float mapSize)
{
	quadtreeNodeMatrices.clear();
	for (auto& node : nodes)
	{
		for (auto& i : node)
		{
			i.update(sim, mapSize);
		}
	}
}

void SimpleRtsQuadtreeNode::subdivide(SimpleRtsSimulation& sim)
{
	auto& system = sim.quadtreeSystem;

	if (!subdivided && (level + 1) < system.levelCount)
	{
		Int newLevel = level + 1;
		system.nodes[newLevel].emplace_back(sim, 2 * x + 0, 2 * y + 0, newLevel, id, system.nodes[newLevel].size() - 1);
		system.nodes[newLevel].emplace_back(sim, 2 * x + 0, 2 * y + 1, newLevel, id, system.nodes[newLevel].size() - 1);
		system.nodes[newLevel].emplace_back(sim, 2 * x + 1, 2 * y + 0, newLevel, id, system.nodes[newLevel].size() - 1);
		system.nodes[newLevel].emplace_back(sim, 2 * x + 1, 2 * y + 1, newLevel, id, system.nodes[newLevel].size() - 1);
		subdivided = true;
	}
}

void SimpleRtsQuadtreeNode::update(SimpleRtsSimulation& sim, const Float mapSize)
{
	auto& system = sim.quadtreeSystem;

	subdivide(sim);

	const Float size     = mapSize / pow(2, level - 1);
	const Vec3  position = Vec3((x * size - mapSize), (y * size - mapSize), 0);
	system.quadtreeNodeMatrices.push_back(matrixFromPositionAndSize(position, size));
}

SimpleRtsQuadtreeNode::SimpleRtsQuadtreeNode(SimpleRtsSimulation& sim, const Int x, const Int y, const Int level, const Int parent, const Int id)
{
	this->x      = x;
	this->y      = y;
	this->id     = id;
	this->level  = level;
	this->parent = parent;
}

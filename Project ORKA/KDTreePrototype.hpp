#pragma once

#include "Prototype.hpp"
#include "Random.hpp"
#include "KDTree.hpp"
#include "Player.hpp"

struct KDTreeBase
{
	Vector<Vec3> points;

	[[nodiscard]] SizeT pointCount() const;
	[[nodiscard]] Float getPoint(SizeT id, SizeT axis) const;
	[[nodiscard]] Vec3  getPoint(SizeT id) const;

	template <class T> Bool kdTreeGetBoundingBox(T&) const
	{
		return false;
	}
};

struct KDTreePrototype final : Prototype
{
	Time         time;
	Vector<Vec3> lines;
	DebugPlayer player;
	Vector<Vec3> velocity;
	KDTreeBase   pointCloud;
	SizeT        pointCount = 1000;

	void action() override;
	void create() override;
	void render(Renderer& r, const Player& player) override;
};

struct KDTreePrototype2 : Prototype
{
	Vector<Vec3> velocity;
	Time         time;
	Vector<Vec3> points;
	SizeT        pointCount = 1;
	Vector<Vec3> lines;

	void action(Renderer& r);
	void create(Renderer& r, Player& player);

	void addPoint()
	{
		points.emplace_back(randomVec3Fast(-100, 100, 0, 0, -100, 100));
	}

	void render(Renderer& r, ResourceManager& rm, Player& player)
	{
		r.useShader("debug");
		r.renderMeshInstanced("sphere", points);

		const KDTree tree(points);

		lines.clear();
		//buildLines(tree, 0);
		//buildLines(tree, 0, 0);
		buildClosestConnections(tree);
		r.lines(lines);
	}

	void buildLines(const KDTree& tree, const ULL nodeID)
	{
		const KDTreeNode& node = tree.nodes[nodeID];

		if(node.a != maxULL)
		{
			lines.push_back(points[node.pointId]);
			lines.push_back(points[tree.nodes[node.a].pointId]);
			buildLines(tree, node.a);
		}
		if(node.b != maxULL)
		{
			lines.push_back(points[tree.nodes[nodeID].pointId]);
			lines.push_back(points[tree.nodes[node.b].pointId]);
			buildLines(tree, node.b);
		}
	}

	void buildLines(const KDTree& tree, const ULL nodeID, const Byte axis)
	{
		const KDTreeNode& node   = tree.nodes[nodeID];
		Vec3              point  = points[node.pointId];
		constexpr Float   extend = 10000.0f;
		if(axis == 0)
		{
			lines.emplace_back(-extend, point.y, point.z);
			lines.emplace_back(+extend, point.y, point.z);
			if(node.a != maxULL) buildLines(tree, node.a, axis + 1);
			if(node.b != maxULL) buildLines(tree, node.b, axis + 1);
			return;
		}
		if(axis == 1)
		{
			lines.emplace_back(point.x, -extend, point.z);
			lines.emplace_back(point.x, +extend, point.z);
			if(node.a != maxULL) buildLines(tree, node.a, axis + 1);
			if(node.b != maxULL) buildLines(tree, node.b, axis + 1);
			return;
		}
		lines.emplace_back(point.x, point.y, -extend);
		lines.emplace_back(point.x, point.y, +extend);
		if(node.a != maxULL) buildLines(tree, node.a, 0);
		if(node.b != maxULL) buildLines(tree, node.b, 0);
	}

	void buildClosestConnections(const KDTree& tree)
	{
		for(auto p : points)
		{
			lines.push_back(p);
			lines.push_back(points[tree.getClosestPointId(p)]);
		}
	}
};

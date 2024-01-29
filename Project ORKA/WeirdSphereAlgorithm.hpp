#pragma once

#include "Math.hpp"
#include "Player.hpp"
#include "Prototype.hpp"
#include "Random.hpp"
#include "ResourceManager.hpp"

struct WeirdSphereNode
{
	Vec3           position = Vec3(0);
	ULL            count    = 1;
	Float          radius   = 1.0f;
	WeirdSphereNode* parent   = nullptr;
	WeirdSphereNode* nw       = nullptr;
	WeirdSphereNode* ne       = nullptr;
	WeirdSphereNode* sw       = nullptr;
	WeirdSphereNode* se       = nullptr;

	WeirdSphereNode(WeirdSphereNode* parent, const Vec3 pos)
	{
		position     = pos;
		count        = 1;
		radius       = 1;
		this->parent = parent;
	}

	void inject(const Vec3 point)
	{
		if(nw || ne || sw || se)
		{
			const Vec3 delta   = position - point;
			const Bool isNorth = delta.y >= 0.0f;
			const Bool isEast  = delta.x >= 0.0f;

			if(isNorth)
			{
				if(isEast)
				{
					if(ne) ne->inject(point);
					else ne = new WeirdSphereNode(this, point);
				}
				else
				{
					if(nw) nw->inject(point);
					else nw = new WeirdSphereNode(this, point);
				}
			}
			else
			{
				if(isEast)
				{
					if(se) se->inject(point);
					else se = new WeirdSphereNode(this, point);
				}
				else
				{
					if(se) se->inject(point);
					else se = new WeirdSphereNode(this, point);
				}
			}
		}
		else
		{
			const Vec3 center = (position + point) / Vec3(2);
			const Vec3 deltaA = position - center;
			if(deltaA.y >= 0.0)
			{
				if(deltaA.x >= 0.0)
				{
					nw = new WeirdSphereNode(this, position);
					se = new WeirdSphereNode(this, point);
				}
				else
				{
					ne = new WeirdSphereNode(this, position);
					sw = new WeirdSphereNode(this, point);
				}
			}
			else
			{
				if(deltaA.x >= 0.0)
				{
					se = new WeirdSphereNode(this, position);
					nw = new WeirdSphereNode(this, point);
				}
				else
				{
					sw = new WeirdSphereNode(this, position);
					ne = new WeirdSphereNode(this, point);
				}
			}
		}

		recalculate();
	}

	void recalculate()
	{
		if(nw || ne || sw || se)
		{
			//reset values
			position = Vec3(0);
			count    = 0;
			radius   = 0;
			//add nodes
			if(nw)
			{
				position += nw->position * Vec3(static_cast<Float>(nw->count));
				count += nw->count;
			}
			if(ne)
			{
				position += ne->position * Vec3(static_cast<Float>(ne->count));
				count += ne->count;
			}
			if(sw)
			{
				position += sw->position * Vec3(static_cast<Float>(sw->count));
				count += sw->count;
			}
			if(se)
			{
				position += se->position * Vec3(static_cast<Float>(se->count));
				count += se->count;
			}

			position /= Vec3(static_cast<Float>(count));

			Float r1 = 0;
			Float r2 = 0;
			Float r3 = 0;
			Float r4 = 0;

			if(nw) r1 = distance(position, nw->position) + nw->radius;
			if(ne) r2 = distance(position, ne->position) + ne->radius;
			if(sw) r3 = distance(position, sw->position) + sw->radius;
			if(se) r4 = distance(position, se->position) + se->radius;

			radius = max(1, max(max(r1, r2), max(r3, r4)));
		}

		if(parent) parent->recalculate();
	}

	void getMatrices(Vector<Matrix>& matrices) const
	{
		const Matrix m = matrixFromPositionAndSize(position, Vec3(radius, radius, 1 / (radius * radius)));
		matrices.push_back(m);
		if(nw) nw->getMatrices(matrices);
		if(ne) ne->getMatrices(matrices);
		if(sw) sw->getMatrices(matrices);
		if(se) se->getMatrices(matrices);
	}

	void getLines(Vector<Vec3>& lines) const
	{
		if(parent)
		{
			lines.push_back(parent->position);
			lines.push_back(position);
		}
		if(nw) nw->getLines(lines);
		if(ne) ne->getLines(lines);
		if(sw) sw->getLines(lines);
		if(se) se->getLines(lines);
	}

	void getMatricesLowest(Vector<Matrix>& matrices) const
	{
		if(nw || ne || sw || se)
		{
		if(nw) nw->getMatricesLowest(matrices);
		if(ne) ne->getMatricesLowest(matrices);
		if(sw) sw->getMatricesLowest(matrices);
		if(se) se->getMatricesLowest(matrices);
		}else
		{
			const Matrix m = matrixFromPositionAndSize(position, Vec3(radius, radius, 1 / (radius * radius)));
			matrices.push_back(m);
		}
	}
};

struct WeirdSphereAlgorithmPrototype : Prototype
{
	const Bool renderLines  = true;
	const Bool renderArrows = false;
	const ULL  numPoints    = 1;

	DebugPlayer      player;
	Vector<Vec3>     points;
	WeirdSphereNode* rootNode = nullptr;

	void addPoint()
	{
		const Vec3 point = randomPointOnSphereFast(100) * Vec3(1,1,0);//-100, 100, -100, 100, 0, 0);
		points.push_back(point);

		if(!rootNode) rootNode = new WeirdSphereNode(nullptr, point);
		else rootNode->inject(point);
	}

	void create() override
	{
		player.camera.setLocation(0,0,150);
		player.camera.setRotation(Vec3(PI,0,0));
		for(ULL i = 0; i < numPoints; i++) addPoint();
	}

	void render(Renderer& r, const Player& player) override
	{
		r.useShader("debug");

		Vector<Matrix> matrices;
		//rootNode->getMatrices(matrices);
		//rootNode->getMatricesLowest(matrices);

		Vector<Vec3> lines;
		rootNode->getLines(lines);

		r.lines(lines);
		//r.renderMeshInstanced(rm, "sphere", matrices);
	}

	void action() override
	{
		addPoint();
	}
};

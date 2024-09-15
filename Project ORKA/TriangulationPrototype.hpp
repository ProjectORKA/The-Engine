#pragma once
#include "Prototype.hpp"
#include "Random.hpp"

struct TriangulationPrototype final : Prototype
{
	struct Connection
	{
		Float distance = 0;
		Index a        = -1;
		Index b        = -1;

		Connection(Index a, Index b, Float distance);
		Bool operator<(const Connection& other) const;
	};

	Vec2Vector       positions;
	Vector<Connection> connections;
	Int                pointCount = 1000;

	void create() override;
	void action() override {}
	void destroy() override {}
	void addConnection(Index a, Index b);
	void render(Renderer& r, const Player& player) override;;
};

#pragma once
#include "Window.hpp"
#include "JobSystem.hpp"


struct Renderer;

struct MindMapConnection
{
	Index a;
	Index b;

	MindMapConnection(Index a, Index b);
};

struct MindMap
{
	Mutex                     mutex;
	Vector<Vec2>              forces;
	Vector<UInt>              numForces;
	Vector<Vec2>              positions;
	Vector<MindMapConnection> connections;
	UInt                      nodeCount = 0;

	void update();
	void addNode();
	void render(Renderer& renderer) const;
	void renderInteractive(Window& window) const;
};

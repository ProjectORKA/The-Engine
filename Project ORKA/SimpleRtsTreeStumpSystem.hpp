#pragma once
#include "Basics.hpp"

struct SimpleRtsTreeStumpSystem
{
	Vector<Vec2> treeStumpPosition;
	UInt         lastTreeStump  = 0;
	UInt         treeStumpCount = 0;
	void render(Renderer& renderer)
	{
		renderer.renderMeshInstanced("TreeStump", treeStumpPosition);
	}
};
#pragma once
#include "Basics.hpp"

struct SimpleRtsTreeStumpSystem
{
	Vec2Vector treeStumpPosition;
	UInt         lastTreeStump  = 0;
	UInt         treeStumpCount = 0;
	void render(Renderer& renderer)
	{
		renderer.renderMeshInstanced("TreeStump", treeStumpPosition);
	}
};
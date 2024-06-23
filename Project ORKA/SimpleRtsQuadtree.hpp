#pragma once
#include "Basics.hpp"
#include "Renderer.hpp"

struct SimpleRtsWorldChunk2DData
{
	Index tree = 0;			//count or index of trees/tree
	Index human = 0;		//...
	Index berryBush = 0;	//...
};

struct SimpleRtsQuadtreeNode
{
	//children
	Index c00 = -1;
	Index c01 = -1;
	Index c10 = -1;
	Index c11 = -1;

	//parent
	Index parent = -1;

	Byte level = 0;

	SimpleRtsWorldChunk2DData data;
};

struct SimpleRtsQuadtree
{
	Vector<Vector<SimpleRtsQuadtreeNode>> levels;

	void create()
	{
		//create root node
		levels.emplace_back().emplace_back();
	}

	void spawnTree()
	{
		
	}

	void render(Renderer & renderer)
	{
		for(auto & level : levels)
		{
			for(auto & node : level)
			{
				renderer.wireframeCube();
			}
		}
	}

	void destroy()
	{
		levels.clear();
	}
};
#pragma once

#include "Basics.hpp"
#include "Random.hpp"
#include "BranchStructure.hpp"

struct Renderer;

void treeGeneration(Vec3Vector& leaves, Vec3Vector& branches, Vector<Index>& connections, Float segmentSize, Float killRadius, Float leafPull);

struct TreeGenerator
{
	BranchStructure tree;
	Vec3Vector    leaves;
	Vec3Vector    branches;
	Bool            treeGen = false;

	void run();
	void render(Renderer& renderer) const;
};

struct Tree
{
	Vec3 position = randomVec3Fast(0.0f, 1.0f);
};

struct TreeSystem
{
	Vector<Tree> trees;
};

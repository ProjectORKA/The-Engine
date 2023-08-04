#pragma once

#include "Basics.hpp"
#include "Random.hpp"
#include "PointCloud.hpp"
#include "BranchStructure.hpp"

struct Renderer;

void treeGeneration(Vector<Vec3>& leaves, Vector<Vec3>& branches, Vector<Index>& connections, Float segmentSize, Float killRadius, Float leafPull);

struct TreeGenerator
{
	BranchStructure tree;
	PointCloud      leaves;
	PointCloud      branches;
	Bool            treeGen = false;

	void run();
	void render(Renderer& renderer);
};

struct Tree
{
	Vec3 position = Vec3(randomFloat(), randomFloat(), randomFloat());
};

struct TreeSystem
{
	Vector<Tree> trees;
};

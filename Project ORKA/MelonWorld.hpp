#pragma once

#include "Basics.hpp"


struct MelonPlayer;
struct Renderer;

#define ROCKGRIDSIZE 128

struct MelonWorld
{
	Mutex      m;
	List<Vec3> nodes;
	List<Vec4> scenery;
	IVec2      offset = IVec2(0);
	Vec4       rocks[ROCKGRIDSIZE][ROCKGRIDSIZE];

	void update(const MelonPlayer& player);
	void render(Renderer& renderer);
	void generateScenery(Float& sceneryPersistencySize, const MelonPlayer& player, const Float& pathWidth);
	void generatePaths(const Float& generationSize, const MelonPlayer& player, const Float& pathWidth, const Float& pathPersistencySize);
};

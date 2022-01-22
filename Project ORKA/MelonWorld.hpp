
#pragma once

#include "Basics.hpp"

struct MelonPlayer;
struct Renderer;

#define ROCKGRIDSIZE 128

struct MelonWorld {
	Mutex m;
	List<Vec3> nodes;
	List<Vec4> scenery;

	Vec4 rocks[ROCKGRIDSIZE][ROCKGRIDSIZE];

	void render(Renderer& renderer);
	void update(MelonPlayer& player);
	void generateScenery(Float& sceneryPersistencySize, MelonPlayer& player, Float& pathWidth);
	void generatePaths(Float& generationSize, MelonPlayer& player, Float& pathWidth, Float& pathPersistencySize);
};
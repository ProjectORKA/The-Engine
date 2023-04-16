
#pragma once

#include "Basics.hpp"
#include "ResourceManager.hpp"

struct MelonPlayer;
struct Renderer;

#define ROCKGRIDSIZE 128

struct MelonWorld {
	Mutex m;
	List<Vec3> nodes;
	List<Vec4> scenery;
	IVec2 offset = IVec2(0);

	Vec4 rocks[ROCKGRIDSIZE][ROCKGRIDSIZE];

	void update(MelonPlayer& player);
	void render(ResourceManager& resourceManager, Renderer& renderer);
	void generateScenery(Float& sceneryPersistencySize, MelonPlayer& player, Float& pathWidth);
	void generatePaths(Float& generationSize, MelonPlayer& player, Float& pathWidth, Float& pathPersistencySize);
};
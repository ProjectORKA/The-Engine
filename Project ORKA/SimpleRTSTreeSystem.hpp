#pragma once

#include "Basics.hpp"

//struct SimpleRTSSimulation;
//
//struct SimpleRTSTreeSystem {
//	UInt maxTreeCount = 0;
//	UInt minTreeCount = 0;
//	Float maxTreeLifeExpectancyInSeconds = 1 * 3;
//	Float minTreeLifeExpectancyInSeconds = 0 * 60;
//	Float fullyGrownMaxTreeSize = 5;
//	Float fullyGrownMinTreeSize = 2;
//	UInt treeCount = 0;
//	Vector<Float> treeAge;
//	Vector<Float> treeSize;
//	Vector<Vec2> treePosition;
//	Vector<Vec2> treeDirection;
//	Vector<Float> lifeExpectancy;
//	Vector<Float> fullyGrownSize;
//
//	Float calculateTreeSize(UInt id);
//	void update(SimpleRTSSimulation& simulation);
//	void create(SimpleRTSSimulation& simulation);
//	void spawnTree(SimpleRTSSimulation& simulation);
//	void cutTree(SimpleRTSSimulation& simulation, UInt id);
//};
//
//struct SimpleRTSTreeSystemRenderer {
//	void render(SimpleRTSTreeSystem & treeSystem, Renderer& renderer);
//};

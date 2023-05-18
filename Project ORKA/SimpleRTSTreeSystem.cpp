//
//#include "SimpleRTSTreeSystem.hpp"
//#include "SimpleRTS.hpp"
//
//Float SimpleRTSTreeSystem::calculateTreeSize(UInt id) {
//	return fullyGrownSize[id] * (treeAge[id] / lifeExpectancy[id]);
//}
//
//void SimpleRTSTreeSystem::update(SimpleRTSSimulation& simulation) {
//	for (UInt i = 0; i < treeCount; i++) {
//		treeAge[i] += simulation.timestep;
//		treeSize[i] = calculateTreeSize(i);
//
//		if (treeAge[i] >= lifeExpectancy[i]) cutTree(i);
//	}
//}
//
//void SimpleRTSTreeSystem::create(SimpleRTSSimulation& simulation) {
//	maxTreeCount = simulation.mapSize * simulation.mapSize / 100;
//	minTreeCount = maxTreeCount;
//
//	while (treeCount < minTreeCount) {
//		spawnTree(simulation);
//	}
//}
//
//void SimpleRTSTreeSystem::spawnTree(SimpleRTSSimulation& simulation) {
//	lifeExpectancy.push_back(random(minTreeLifeExpectancyInSeconds, maxTreeLifeExpectancyInSeconds));
//	treeAge.push_back(randomFloat(minTreeLifeExpectancyInSeconds, lifeExpectancy.last()));
//	treeDirection.push_back(randomUnitVec2());
//	treePosition.push_back(randomVec2(-simulation.mapSize, simulation.mapSize));
//	fullyGrownSize.push_back(random(fullyGrownMinTreeSize, fullyGrownMaxTreeSize));
//	treeSize.push_back(calculateTreeSize(treeCount));
//	treeCount++;
//}
//
//void SimpleRTSTreeSystem::cutTree(SimpleRTSSimulation& simulation, UInt id) {
//	treeSize[id] = 0;
//	treeAge[id] = 0;
//	Vec2 newTreePosition = treePosition[randomInt(treePosition.size())];
//	loopWithinCentered(newTreePosition, simulation.mapSize);
//	while (isWithinDistanceOfOtherPoints(newTreePosition, treePosition, 1.0f)) {
//		newTreePosition += normalize(randomVec2(-1, 1)) * 3.0f;
//		loopWithinCentered(newTreePosition, simulation.mapSize);
//	}
//	//spawn tree stump here
//	//treeStumpPosition[lastTreeStump] = treePosition[id];
//	//lastTreeStump++;
//	//if (lastTreeStump >= treeStumpPosition.size()) lastTreeStump = 0;
//	treePosition[id] = newTreePosition;
//}
//
//void SimpleRTSTreeSystemRenderer::render(ResourceManager& resourceManager, SimpleRTSTreeSystem& treeSystem, Renderer& renderer) {
//	renderer.matrixSystem.matrixArray(treeSystem.treePosition, treeSystem.treeDirection, treeSystem.treeSize);
//	renderer.renderMeshInstanced(resourceManager, "Tree");
//}

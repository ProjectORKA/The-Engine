
#include "SimpleRTS.hpp"
#include "Engine.hpp"
#include "UserInterface.hpp"

void SimpleRTSRenderer::update(Window& window) {
	player.update(window);
}
void SimpleRTSRenderer::render(Engine& engine, Window& window, TiledRectangle area) {
	Renderer& renderer = window.renderer;
	
	mutex.lock();

	renderer.setWireframeMode();
	renderer.setCulling(true);
	renderer.clearDepth();

	//sky
	renderer.setDepthTest(false);
	renderer.useShader(engine, "color");
	renderer.fill(Color(0.207143, 0.722031, 1.0f, 1));
	renderer.uniforms().sunDir(Vec4(normalize(Vec3(1)),1));
	renderer.renderMesh(engine, "fullScreenQuad");
	renderer.setDepthTest(true);

	renderer.fill(Color(1.0f));

	//prepare rendering scene
	renderer.uniforms().mMatrix() = Matrix(1);
	renderer.useShader(engine, "simpleRTS");
	player.render(engine, window);



	//ground plane
	renderer.uniforms().mMatrix(matrixFromScale(SIMPLERTS_MAPSIZE));
	renderer.renderMesh(engine, "FloatingIsland");

	////render trees that are about to be cut
	//for (UInt i = 0; i < sim->humanCount; i++) {
	//	if (sim->humanState[i] == SimpleRTSSimulation::SimpleRTSHumanState::LookingForWood) {
	//		renderer.uniforms().mMatrix(matrixFromLocation(sim->humanTargetpositions[i]));
	//		renderer.renderMesh("Tree");
	//	}
	//}

	//prepare for instancing
	renderer.uniforms().mMatrix(Matrix(1));


	sim->render(engine, renderer);

	////berry bushes
	//renderer.matrixSystem.matrixArray(sim->bushPosition, sim->bushDirection);
	//renderer.renderMeshInstanced("Bush");

	////berry bushes
	//renderer.matrixSystem.matrixArray(sim->berryBushPosition, sim->berryBushDirection);
	//renderer.renderMeshInstanced("BerryBush");

	////render tree stumps
	//renderer.matrixSystem.matrixArray(sim->treeStumpPosition);
	//renderer.renderMeshInstanced("TreeStump");

	////render humans
	//renderer.matrixSystem.matrixArray(sim->humanPositions, sim->humanDirection);
	//renderer.renderMeshInstanced("Human");

	////render bases
	//renderer.matrixSystem.matrixArray(sim->basePosition);
	//renderer.renderMeshInstanced("Base");

	////render rabbits
	//renderer.matrixSystem.matrixArray(sim->rabbitPosition, sim->rabbitVelocity);
	//renderer.renderMeshInstanced("Rabbit");

	//ui
	renderer.setDepthTest(false);
	renderer.screenSpace();
	renderer.renderText(engine, String(toString(1.0f / renderer.time.delta)), Vec2(50), fonts.heading);

	////////////////////////

	mutex.unlock();
}
void SimpleRTSRenderer::inputEvent(Window& window, InputEvent input) {

	if (input == enter) window.captureCursor();
	if (input == exit) window.uncaptureCursor();

	player.inputEvent(window, input);
}


//void SimpleRTSSimulation::spawnTreeStump() {
//	treeStumpPosition.push_back(randomVec2(-SIMPLERTS_MAPSIZE, SIMPLERTS_MAPSIZE));
//	treeStumpCount++;
//}
//Index SimpleRTSSimulation::getCuttableTree(Vec2 position) {
//	return idOfClosestPointInLoopingSpace(position, treePosition, SIMPLERTS_MAPSIZE);
//}
//void SimpleRTSSimulation::spawnBase(UInt clanID) {
//	spawnBase(clanID, randomVec2(-SIMPLERTS_MAPSIZE, SIMPLERTS_MAPSIZE));
//}
//void SimpleRTSSimulation::spawnBase(UInt clanID, Vec2 position) {
//	baseClanID.push_back(clanID);
//	basePosition.push_back(position);
//	baseCount++;
//}
//void SimpleRTSSimulation::spawnHouse(UInt clanID) {
//
//	Index currentID;
//	do {
//		currentID = randomInt(baseClanID.size());
//	} while (baseClanID[currentID] != clanID);
//
//	spawnBase(clanID, basePosition[currentID] + randomUnitVec2() * Vec2(10));
//}
//void SimpleRTSSimulation::produceWorker(UInt baseID) {
//	if (clanFood[baseClanID[baseID]] > 0 && clanWood[baseClanID[baseID]] >= 5) {
//		clanFood[baseClanID[baseID]]--;
//		clanWood[baseClanID[baseID]] -= 5;
//		spawnHumanInBase(baseID);
//	}
//}
//UInt SimpleRTSSimulation::getIndexOfClosestBaseOfClan(UInt clanID, Vec2 position) {
//	Float smallestDistance = distanceToPointInLoopingSpace(basePosition[0], position, SIMPLERTS_MAPSIZE);
//	UInt indexOfClosestPoint = 0;
//	for (UInt i = 1; i < baseCount; i++) {
//		Float currentDistance = distanceToPointInLoopingSpace(basePosition[i], position, SIMPLERTS_MAPSIZE);
//		if (baseClanID[i] == clanID && currentDistance < smallestDistance) {
//			smallestDistance = currentDistance;
//			indexOfClosestPoint = i;
//		}
//	}
//	return indexOfClosestPoint;
//}
//void SimpleRTSSimulation::updateHumans() {
//	for (UInt humanID = 0; humanID < humanCount; humanID++) {
//		switch (humanState[humanID]) {
//		case SimpleRTSHumanState::Delivering: deliverItem(humanID); break;
//		case SimpleRTSHumanState::LookingForWood: chopWood(humanID); break;
//		case SimpleRTSHumanState::HuntingRabbit: huntRabbit(humanID); break;
//		default: break;
//		}
//	}
//}
//void SimpleRTSSimulation::chopWood(Index id) {
//	if (humanPositions[id] != humanTargetpositions[id]) {
//		moveHuman(id);
//	}
//	else {
//		humanCarriedItem[id] = SimpleRTSItem::Wood;
//		humanTargetpositions[id] = basePosition[getIndexOfClosestBaseOfClan(humanClan[id], humanPositions[id])];
//		humanState[id] = SimpleRTSHumanState::Delivering;
//	}
//}
//void SimpleRTSSimulation::moveHuman(Index id) {
//	Vec2 delta = deltaInLoopingSpace(humanPositions[id], humanTargetpositions[id], SIMPLERTS_MAPSIZE);
//	humanDirection[id] = normalize(delta);
//	Vec2 step = humanDirection[id] * timestep * humanSpeed[id];
//	if (length(delta) < length(step)) humanPositions[id] += delta;
//	else humanPositions[id] += step;
//	loopWithin(humanPositions[id], SIMPLERTS_MAPSIZE);
//}
//void SimpleRTSSimulation::huntRabbit(Index id) {
//	humanTargetpositions[id] = rabbitPosition[humanTargetID[id]];
//	moveHuman(id);
//	if (humanPositions[id] == humanTargetpositions[id]) {
//		killRabbit(humanTargetID[id]);
//		humanCarriedItem[id] = SimpleRTSItem::Food;
//		humanState[id] = SimpleRTSHumanState::Delivering;
//
//		for (Int i = 0; i < humanCount; i++) {
//			if (humanState[i] == SimpleRTSHumanState::HuntingRabbit) {
//				humanTargetID[i] = getClosestRabbit(humanPositions[i]);
//			}
//		}
//
//		humanTargetpositions[id] = basePosition[getIndexOfClosestBaseOfClan(humanClan[id], humanPositions[id])];
//	}
//
//}
//void SimpleRTSSimulation::deliverItem(Index id) {
//	if (humanPositions[id] != humanTargetpositions[id]) {
//		moveHuman(id);
//	}
//	else {
//		switch (humanCarriedItem[id]) {
//		case SimpleRTSItem::Wood:
//			// add wood to clan
//			clanWood[humanClan[id]]++;
//			break;
//		case SimpleRTSItem::Food:
//			//add food to clan
//			clanFood[humanClan[id]]++;
//		}
//		humanCarriedItem[id] = SimpleRTSItem::Nothing;
//
//		switch (getLowestResource(humanClan[id])) {
//		case SimpleRTSItem::Food:
//			humanTargetID[id] = getClosestRabbit(humanPositions[id]);
//			humanState[id] = SimpleRTSHumanState::HuntingRabbit;
//			break;
//		case SimpleRTSItem::Wood:
//			// get next tree to cut
//			Index closestTreeID = getCuttableTree(humanPositions[id]);
//			//set at target
//			humanTargetpositions[id] = treePosition[closestTreeID];
//			//change worker state
//			humanState[id] = SimpleRTSHumanState::LookingForWood;
//			//precut tree (its an optimization, renderer will render fake tree instead)
//			cutTree(closestTreeID);
//			break;
//		};
//	}
//}
//void SimpleRTSSimulation::spawnRabbit() {
//	rabbitPosition.push_back(randomVec2(-SIMPLERTS_MAPSIZE, SIMPLERTS_MAPSIZE));
//	rabbitVelocity.push_back(randomUnitVec2());
//	rabbitCount++;
//}
//void SimpleRTSSimulation::updateRabbits() {
//	for (Int i = 0; i < rabbitCount; i++) {
//		rabbitVelocity[i] = normalize(rabbitVelocity[i] + rabbitJitter * timestep * randomUnitVec2());
//		rabbitPosition[i] += rabbitSpeed * timestep * rabbitVelocity[i];
//		loopWithin(rabbitPosition[i], SIMPLERTS_MAPSIZE);
//	}
//}
//void SimpleRTSSimulation::killRabbit(Index id) {
//	rabbitPosition[id] = randomVec2(-SIMPLERTS_MAPSIZE, SIMPLERTS_MAPSIZE);
//	rabbitVelocity[id] = randomUnitVec2();
//}
//Index SimpleRTSSimulation::getClosestRabbit(Vec2 pos) {
//	return idOfClosestPointInLoopingSpace(pos, rabbitPosition, SIMPLERTS_MAPSIZE);
//}
//void SimpleRTSSimulation::spawnClan() {
//	clanWorkerCount.push_back(0);
//	clanFood.push_back(0);
//	clanWood.push_back(0);
//	spawnBase(clanCount);
//	clanCount++;
//}
//void SimpleRTSSimulation::updateBases() {
//	for (UInt baseID = 0; baseID < baseCount; baseID++) {
//		spawnHouse(baseClanID[baseID]);
//	}
//}
//SimpleRTSSimulation::SimpleRTSItem SimpleRTSSimulation::getLowestResource(Index clanID) {
//	UInt res = -1;
//	SimpleRTSItem resItem = SimpleRTSItem::Food;
//
//	if (clanFood[clanID] < res) {
//		res = clanFood[clanID];
//		resItem = SimpleRTSItem::Food;
//	}
//	if (clanWood[clanID] < res) {
//		res = clanWood[clanID];
//		resItem = SimpleRTSItem::Wood;
//	}
//	return resItem;
//}

SimpleRTSSimulation::SimpleRTSSimulation() {

	trees.create(*this);




	//generateTerrain();

	//spawnHuman();

	//while (berryBushPosition.size() < 10000) {
	//	berryBushPosition.push_back(randomVec2(-SIMPLERTS_MAPSIZE, SIMPLERTS_MAPSIZE));
	//	berryBushDirection.push_back(randomUnitVec2());
	//}

	//while (bushPosition.size() < 10000) {
	//	bushPosition.push_back(randomVec2(-SIMPLERTS_MAPSIZE, SIMPLERTS_MAPSIZE));
	//	bushDirection.push_back(randomUnitVec2());
	//}


	//while (treeStumpCount < 3000) {
	//	spawnTreeStump();
	//}

	//while (rabbitCount < 300) {
	//	spawnRabbit();
	//}
}
void SimpleRTSSimulation::update(Float timestep) {
	trees.update(*this);

	//treeSimulation.update();
	//humanitySimulation;
	//bushSimulation;
	//houseSimulation;
	//rabbitSimulation;
}

void Trees::update(SimpleRTSSimulation& simulation) {
	for (UInt i = 0; i < treeCount; i++) {
		treeAge[i] += simulation.timestep;
		treeSize[i] = calculateTreeSize(i);

		if (treeAge[i] >= lifeExpectancy[i]) cutTree(i);
	}
}

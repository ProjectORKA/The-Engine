#include "SimpleRTS.hpp"
#include "ResourceManager.hpp"
#include "UserInterface.hpp"

//void SimpleRTSSimulation::spawnTreeStump() {
//	treeStumpPosition.push_back(randomVec2(-SIMPLERTS_MAPSIZE, SIMPLERTS_MAPSIZE));
//	treeStumpCount++;
//}
//Index SimpleRTSSimulation::getCuttableTree(Vec2 position) {
//	return idOfClosestPointInLoopingSpace(position, treePosition, SIMPLERTS_MAPSIZE);
//}
//void SimpleRTSSimulation::spawnBase(UInt clanId) {
//	spawnBase(clanId, randomVec2(-SIMPLERTS_MAPSIZE, SIMPLERTS_MAPSIZE));
//}
//void SimpleRTSSimulation::spawnBase(UInt clanId, Vec2 position) {
//	baseClanId.push_back(clanId);
//	basePosition.push_back(position);
//	baseCount++;
//}
//void SimpleRTSSimulation::spawnHouse(UInt clanId) {
//
//	Index currentId;
//	do {
//		currentId = randomInt(baseClanId.size());
//	} while (baseClanId[currentId] != clanId);
//
//	spawnBase(clanId, basePosition[currentId] + randomUnitVec2() * Vec2(10));
//}
//void SimpleRTSSimulation::produceWorker(UInt baseId) {
//	if (clanFood[baseClanId[baseId]] > 0 && clanWood[baseClanId[baseId]] >= 5) {
//		clanFood[baseClanId[baseId]]--;
//		clanWood[baseClanId[baseId]] -= 5;
//		spawnHumanInBase(baseId);
//	}
//}
//UInt SimpleRTSSimulation::getIndexOfClosestBaseOfClan(UInt clanId, Vec2 position) {
//	Float smallestDistance = distanceToPointInLoopingSpace(basePosition[0], position, SIMPLERTS_MAPSIZE);
//	UInt indexOfClosestPoint = 0;
//	for (UInt i = 1; i < baseCount; i++) {
//		Float currentDistance = distanceToPointInLoopingSpace(basePosition[i], position, SIMPLERTS_MAPSIZE);
//		if (baseClanId[i] == clanId && currentDistance < smallestDistance) {
//			smallestDistance = currentDistance;
//			indexOfClosestPoint = i;
//		}
//	}
//	return indexOfClosestPoint;
//}
//void SimpleRTSSimulation::updateHumans() {
//	for (UInt humanId = 0; humanId < humanCount; humanId++) {
//		switch (humanState[humanId]) {
//		case SimpleRTSHumanState::Delivering: deliverItem(humanId); break;
//		case SimpleRTSHumanState::LookingForWood: chopWood(humanId); break;
//		case SimpleRTSHumanState::HuntingRabbit: huntRabbit(humanId); break;
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
//	humanTargetpositions[id] = rabbitPosition[humanTargetId[id]];
//	moveHuman(id);
//	if (humanPositions[id] == humanTargetpositions[id]) {
//		killRabbit(humanTargetId[id]);
//		humanCarriedItem[id] = SimpleRTSItem::Food;
//		humanState[id] = SimpleRTSHumanState::Delivering;
//
//		for (Int i = 0; i < humanCount; i++) {
//			if (humanState[i] == SimpleRTSHumanState::HuntingRabbit) {
//				humanTargetId[i] = getClosestRabbit(humanPositions[i]);
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
//			humanTargetId[id] = getClosestRabbit(humanPositions[id]);
//			humanState[id] = SimpleRTSHumanState::HuntingRabbit;
//			break;
//		case SimpleRTSItem::Wood:
//			// get next tree to cut
//			Index closestTreeId = getCuttableTree(humanPositions[id]);
//			//set at target
//			humanTargetpositions[id] = treePosition[closestTreeId];
//			//change worker state
//			humanState[id] = SimpleRTSHumanState::LookingForWood;
//			//precut tree (its an optimization, renderer will render fake tree instead)
//			cutTree(closestTreeId);
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
//	for (UInt baseId = 0; baseId < baseCount; baseId++) {
//		spawnHouse(baseClanId[baseId]);
//	}
//}
//SimpleRTSSimulation::SimpleRTSItem SimpleRTSSimulation::getLowestResource(Index clanId) {
//	UInt res = -1;
//	SimpleRTSItem resItem = SimpleRTSItem::Food;
//
//	if (clanFood[clanId] < res) {
//		res = clanFood[clanId];
//		resItem = SimpleRTSItem::Food;
//	}
//	if (clanWood[clanId] < res) {
//		res = clanWood[clanId];
//		resItem = SimpleRTSItem::Wood;
//	}
//	return resItem;
//}

void SimpleRTS::run()
{
	resourceManager.create();
	ui.create();
	sim.start(resourceManager);
	renderer.sim = &sim;
	ui.window("Simple RTS", Area(1920, 1080), true, true, WindowState::Windowed, renderer, resourceManager);
	ui.run();
	sim.stop();
}

Vec2 randomSpawnPosition()
{
	return randomVec2(SIMPLERTS_MAPSIZE);
}

void SimpleRTSSimulation::destroy() {}

void SimpleRTSRenderer::destroy(Window& window) {}

void SimpleRTSSimulation::update(Float delta)
{
	//trees.update(*this);
	//treeSimulation.update();
	//humanitySimulation;
	//bushSimulation;
	//houseSimulation;
	//rabbitSimulation;
}

void SimpleRTSRenderer::update(Window& window)
{
	terrainRenderingSystem.update(sim->terrainSystem, player.camera.getLocation());
	player.update(window);
}

void SimpleRTSSimulation::create(ResourceManager& resourceManager)
{
	//trees.create(*this);

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

void SimpleRTSRenderer::inputEvent(Window& window, const InputEvent input)
{
	if(input == enter) window.captureCursor();
	if(input == exit) window.unCaptureCursor();
	if(input == toggleWireframe) wireframeMode = !wireframeMode;
	player.inputEvent(window, input);
}

void SimpleRTSRenderer::create(ResourceManager& resourceManager, Window& window)
{
	terrainRenderingSystem.create(resourceManager);
}

void SimpleRTSSimulation::render(ResourceManager& resourceManager, Renderer& renderer) const
{
	//trees.render(resourceManager, renderer);
}

void SimpleRTSRenderer::render(ResourceManager& resourceManager, Window& window, TiledRectangle area)
{
	Renderer& renderer = window.renderer;

	mutex.lock();

	renderer.setWireframeMode();
	renderer.setCulling(true);
	renderer.clearBackground(Color(0, 0, 0, 1));

	//sky
	renderer.setDepthTest(false);
	renderer.useShader(resourceManager, "color");
	renderer.fill(Color(0.207143, 0.722031, 1.0f, 1));
	renderer.uniforms().setSunDir(Vec4(normalize(Vec3(1)), 1));
	renderer.renderMesh(resourceManager, "fullScreenQuad");
	renderer.setDepthTest(true);

	renderer.fill(Color(1.0f));

	renderer.setWireframeMode(wireframeMode);

	//prepare rendering scene
	renderer.uniforms().setMMatrix(Matrix(1));
	renderer.useShader(resourceManager, "simpleRTS");
	player.render(resourceManager, window);

	renderer.fill(Color(0.1, 0.4, 0.0f, 1));
	terrainRenderingSystem.render(renderer);

	////render trees that are about to be cut
	//for (UInt i = 0; i < sim->humanCount; i++) {
	//	if (sim->humanState[i] == SimpleRTSSimulation::SimpleRTSHumanState::LookingForWood) {
	//		renderer.uniforms().mMatrix(matrixFromLocation(sim->humanTargetpositions[i]));
	//		renderer.renderMesh("Tree");
	//	}
	//}

	//prepare for instancing
	renderer.uniforms().setMMatrix(Matrix(1));

	sim->render(resourceManager, renderer);

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
	renderer.textRenderSystem.setStyle(fonts.heading);
	renderer.textRenderSystem.render(resourceManager, renderer, toString(1.0f / renderer.time.delta), Vec2(50));
	renderer.textRenderSystem.setStyle(fonts.debug);
	////////////////////////

	mutex.unlock();
}

void SimpleRTSRenderer::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {}

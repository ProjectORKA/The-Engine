
#include "SimpleRTS.hpp"
#include "Scene.hpp"

void SimpleRTSRenderer::update(Renderer& renderer) {
	player.speed = pow(1.2, player.speedExponent);

	if (forward.pressed)	player.accelerationVector += player.camera.forwardVector;
	if (backward.pressed)	player.accelerationVector -= player.camera.forwardVector;
	if (upward.pressed)		player.accelerationVector += player.camera.upVector;
	if (downward.pressed)	player.accelerationVector -= player.camera.upVector;
	if (right.pressed)		player.accelerationVector += player.camera.rightVector;
	if (left.pressed)		player.accelerationVector -= player.camera.rightVector;

	player.update(renderer);
}
SimpleRTSRenderer::SimpleRTSRenderer(SimpleRTSSimulation& sim) {
	this->sim = &sim;
}
void SimpleRTSRenderer::mouseIsMoving(Window& window, IVec2 position) {
	if (inputManager.isCapturing(window))player.camera.rotate(Vec2(position) * Vec2(mouseSensitivity));
}
void SimpleRTSRenderer::render(TiledRectangle area, Renderer& renderer) {
	mutex.lock();

	renderer.setWireframeMode();
	renderer.setCulling(true);
	renderer.clearDepth();

	//sky
	renderer.setDepthTest(false);
	renderer.useShader("color");
	renderer.fill(Color(0.207143, 0.722031, 1.0f, 1));
	renderer.uniforms().sunDir(Vec4(normalize(Vec3(1)),1));
	renderer.renderMesh("fullScreenQuad");
	renderer.setDepthTest(true);

	renderer.fill(Color(1.0f));

	//prepare rendering scene
	renderer.uniforms().mMatrix() = Matrix(1);
	renderer.useShader("simpleRTS");
	player.render(renderer);

	//ground plane
	renderer.uniforms().mMatrix(matrixFromScale(SIMPLERTS_MAPSIZE));
	renderer.renderMesh("FloatingIsland");

	//render trees that are about to be cut
	for (UInt i = 0; i < sim->humanCount; i++) {
		if (sim->humanState[i] == SimpleRTSSimulation::SimpleRTSHumanState::LookingForWood) {
			renderer.uniforms().mMatrix(matrixFromLocation(sim->humanTargetpositions[i]));
			renderer.renderMesh("Tree");
		}
	}

	//prepare for instancing
	renderer.uniforms().mMatrix(Matrix(1));

	//trees
	renderer.matrixSystem.matrixArray(sim->treePosition, sim->treeDirection, sim->treeSize);
	renderer.renderMeshInstanced("Tree");

	//berry bushes
	renderer.matrixSystem.matrixArray(sim->bushPosition, sim->bushDirection);
	renderer.renderMeshInstanced("Bush");

	//berry bushes
	renderer.matrixSystem.matrixArray(sim->berryBushPosition, sim->berryBushDirection);
	renderer.renderMeshInstanced("BerryBush");

	//render tree stumps
	renderer.matrixSystem.matrixArray(sim->treeStumpPosition);
	renderer.renderMeshInstanced("TreeStump");

	//render humans
	renderer.matrixSystem.matrixArray(sim->humanPositions, sim->humanDirection);
	renderer.renderMeshInstanced("Human");

	//render bases
	renderer.matrixSystem.matrixArray(sim->basePosition);
	renderer.renderMeshInstanced("Base");

	//render rabbits
	renderer.matrixSystem.matrixArray(sim->rabbitPosition, sim->rabbitVelocity);
	renderer.renderMeshInstanced("Rabbit");

	//ui
	renderer.setDepthTest(false);
	renderer.screenSpace();
	renderer.renderText(String(toString(1.0f / renderer.time.delta)), Vec2(50), fonts.heading);

	////////////////////////

	mutex.unlock();
}
void SimpleRTSRenderer::mouseIsScrolled(Window& window, Double xAxis, Double yAxis) {
	player.speedExponent += yAxis;
}
void SimpleRTSRenderer::buttonIsPressed(Window& window, Key key, ActionState action, Int modifiers) {
	Bool pressed = (action != ActionState::Release);
	switch (key) {
	case Key::F: if (pressed) window.renderer.wireframeMode = !window.renderer.wireframeMode;
		break;
	case Key::W: forward.pressed = pressed;
		break;
	case Key::S: backward.pressed = pressed;
		break;
	case Key::A: left.pressed = pressed;
		break;
	case Key::D: right.pressed = pressed;
		break;
	case Key::Q: downward.pressed = pressed;
		break;
	case Key::E: upward.pressed = pressed;
		break;
	default:
		break;
	}
}
void SimpleRTSRenderer::mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers) {
	if (button == MouseButton::LEFT && action == ActionState::Press) inputManager.captureCursor(window);
	if (button == MouseButton::RIGHT && action == ActionState::Press) inputManager.uncaptureCursor(window);
}

void SimpleRTSSimulation::spawnTree() {
	treeAge.push_back(0.0f);
	treeSize.push_back(randomFloat(0.75, 1.5));
	treeDirection.push_back(randomUnitVec2());
	treePosition.push_back(randomVec2(-SIMPLERTS_MAPSIZE, SIMPLERTS_MAPSIZE));
	treeCount++;
}

void SimpleRTSSimulation::spawnTreeStump() {
	treeStumpPosition.push_back(randomVec2(-SIMPLERTS_MAPSIZE, SIMPLERTS_MAPSIZE));
	treeStumpCount++;
}

void SimpleRTSSimulation::cutTree(Index id) {
	treeAge[id] = 0;
	Vec2 newTreePosition = treeStumpPosition[randomInt(treeStumpPosition.size())];
	loopWithin(newTreePosition, SIMPLERTS_MAPSIZE);
	while (isWithinDistanceOfOtherPoints(newTreePosition, treePosition, 1.0f)) {
		newTreePosition += normalize(randomVec2(-1, 1)) * 3.0f;
		loopWithin(newTreePosition, SIMPLERTS_MAPSIZE);
	}
	//spawn tree stump here
	treeStumpPosition[lastTreeStump] = treePosition[id];
	lastTreeStump++;
	if (lastTreeStump >= treeStumpPosition.size()) lastTreeStump = 0;
	treePosition[id] = newTreePosition;
}

Index SimpleRTSSimulation::getCuttableTree(Vec2 position) {
	return idOfClosestPointInLoopingSpace(position, treePosition, SIMPLERTS_MAPSIZE);
}

void SimpleRTSSimulation::spawnBase(UInt clanID) {
	baseClanID.push_back(clanID);
	basePosition.push_back(randomVec2(-SIMPLERTS_MAPSIZE, SIMPLERTS_MAPSIZE));
	spawnHumanInBase(baseCount);
	baseCount++;
}

void SimpleRTSSimulation::produceWorker(UInt baseID) {
	if (clanFood[baseClanID[baseID]] > 0 && clanWood[baseClanID[baseID]] >= 5) {
		clanFood[baseClanID[baseID]]--;
		clanWood[baseClanID[baseID]] -= 5;;
		spawnHumanInBase(baseID);
	}
}

UInt SimpleRTSSimulation::getIndexOfClosestBaseOfClan(UInt clanID, Vec2 position) {
	Float smallestDistance = distanceToPointInLoopingSpace(basePosition[0], position, SIMPLERTS_MAPSIZE);
	UInt indexOfClosestPoint = 0;
	for (UInt i = 1; i < baseCount; i++) {
		Float currentDistance = distanceToPointInLoopingSpace(basePosition[i], position, SIMPLERTS_MAPSIZE);
		if (baseClanID[i] == clanID && currentDistance < smallestDistance) {
			smallestDistance = currentDistance;
			indexOfClosestPoint = i;
		}
	}
	return indexOfClosestPoint;
}

void SimpleRTSSimulation::updateHumans() {
	for (UInt humanID = 0; humanID < humanCount; humanID++) {
		switch (humanState[humanID]) {
		case SimpleRTSHumanState::Delivering: deliverItem(humanID); break;
		case SimpleRTSHumanState::LookingForWood: chopWood(humanID); break;
		case SimpleRTSHumanState::HuntingRabbit: huntRabbit(humanID); break;
		default: break;
		}
	}
}

void SimpleRTSSimulation::chopWood(Index id) {
	if (humanPositions[id] != humanTargetpositions[id]) {
		moveHuman(id);
	}
	else {
		humanCarriedItem[id] = SimpleRTSItem::Wood;
		humanTargetpositions[id] = basePosition[getIndexOfClosestBaseOfClan(humanClan[id], humanPositions[id])];
		humanState[id] = SimpleRTSHumanState::Delivering;
	}
}

void SimpleRTSSimulation::moveHuman(Index id) {
	Vec2 delta = deltaInLoopingSpace(humanPositions[id], humanTargetpositions[id], SIMPLERTS_MAPSIZE);
	humanDirection[id] = normalize(delta);
	Vec2 step = humanDirection[id] * timestep * humanSpeed[id];
	if (length(delta) < length(step)) humanPositions[id] += delta;
	else humanPositions[id] += step;
	loopWithin(humanPositions[id], SIMPLERTS_MAPSIZE);
}

void SimpleRTSSimulation::huntRabbit(Index id) {
	humanTargetpositions[id] = rabbitPosition[humanTargetID[id]];
	moveHuman(id);
	if (humanPositions[id] == humanTargetpositions[id]) {
		killRabbit(humanTargetID[id]);
		humanCarriedItem[id] = SimpleRTSItem::Food;
		humanState[id] = SimpleRTSHumanState::Delivering;

		for (Int i = 0; i < humanCount; i++) {
			if (humanState[i] == SimpleRTSHumanState::HuntingRabbit) {
				humanTargetID[i] = getClosestRabbit(humanPositions[i]);
			}
		}

		humanTargetpositions[id] = basePosition[getIndexOfClosestBaseOfClan(humanClan[id], humanPositions[id])];
	}

}

void SimpleRTSSimulation::deliverItem(Index id) {
	if (humanPositions[id] != humanTargetpositions[id]) {
		moveHuman(id);
	}
	else {
		switch (humanCarriedItem[id]) {
		case SimpleRTSItem::Wood:
			// add wood to clan
			clanWood[humanClan[id]]++;
			break;
		case SimpleRTSItem::Food:
			//add food to clan
			clanFood[humanClan[id]]++;
		}
		humanCarriedItem[id] = SimpleRTSItem::Nothing;

		switch (getLowestResource(humanClan[id])) {
		case SimpleRTSItem::Food:
			humanTargetID[id] = getClosestRabbit(humanPositions[id]);
			humanState[id] = SimpleRTSHumanState::HuntingRabbit;
			break;
		case SimpleRTSItem::Wood:
			// get next tree to cut
			Index closestTreeID = getCuttableTree(humanPositions[id]);
			//set at target
			humanTargetpositions[id] = treePosition[closestTreeID];
			//change worker state
			humanState[id] = SimpleRTSHumanState::LookingForWood;
			//precut tree (its an optimization, renderer will render fake tree instead)
			cutTree(closestTreeID);
			break;
		};
	}
}

void SimpleRTSSimulation::spawnHumanInBase(Index baseID) {
	humanTargetID.push_back(0);


	humanClan.push_back(baseClanID[baseID]);
	humanSpeed.push_back(randomFloat(8.1, 15));
	humanDirection.push_back(randomUnitVec2());
	humanPositions.push_back(basePosition[baseID]);
	humanCarriedItem.push_back(SimpleRTSItem::Nothing);
	humanTargetpositions.push_back(basePosition[baseID]);
	humanState.push_back(SimpleRTSHumanState::LookingForWood);

	clanWorkerCount[baseClanID[baseID]]++;
	humanCount++;
}

void SimpleRTSSimulation::spawnRabbit() {
	rabbitPosition.push_back(randomVec2(-SIMPLERTS_MAPSIZE, SIMPLERTS_MAPSIZE));
	rabbitVelocity.push_back(randomUnitVec2());
	rabbitCount++;
}

void SimpleRTSSimulation::updateRabbits() {
	for (Int i = 0; i < rabbitCount; i++) {
		rabbitVelocity[i] = normalize(rabbitVelocity[i] + rabbitJitter * timestep * randomUnitVec2());
		rabbitPosition[i] += rabbitSpeed * timestep * rabbitVelocity[i];
		loopWithin(rabbitPosition[i], SIMPLERTS_MAPSIZE);
	}
}

void SimpleRTSSimulation::killRabbit(Index id) {
	rabbitPosition[id] = randomVec2(-SIMPLERTS_MAPSIZE, SIMPLERTS_MAPSIZE);
	rabbitVelocity[id] = randomUnitVec2();
}

Index SimpleRTSSimulation::getClosestRabbit(Vec2 pos) {
	return idOfClosestPointInLoopingSpace(pos, rabbitPosition, SIMPLERTS_MAPSIZE);
}

void SimpleRTSSimulation::spawnClan() {
	clanWorkerCount.push_back(0);
	clanFood.push_back(0);
	clanWood.push_back(0);
	spawnBase(clanCount);
	clanCount++;
}

void SimpleRTSSimulation::updateBases() {
	for (UInt baseID = 0; baseID < baseCount; baseID++) {
		produceWorker(baseID);
	}
}

SimpleRTSSimulation::SimpleRTSItem SimpleRTSSimulation::getLowestResource(Index clanID) {
	UInt res = -1;
	SimpleRTSItem resItem = SimpleRTSItem::Food;

	if (clanFood[clanID] < res) {
		res = clanFood[clanID];
		resItem = SimpleRTSItem::Food;
	}
	if (clanWood[clanID] < res) {
		res = clanWood[clanID];
		resItem = SimpleRTSItem::Wood;
	}
	return resItem;
}

SimpleRTSSimulation::SimpleRTSSimulation() {
	generateTerrain();

	spawnClan();
	spawnClan();
	spawnClan();

	while (berryBushPosition.size() < 10000) {
		berryBushPosition.push_back(randomVec2(-SIMPLERTS_MAPSIZE, SIMPLERTS_MAPSIZE));
		berryBushDirection.push_back(randomUnitVec2());
	}

	while (bushPosition.size() < 10000) {
		bushPosition.push_back(randomVec2(-SIMPLERTS_MAPSIZE, SIMPLERTS_MAPSIZE));
		bushDirection.push_back(randomUnitVec2());
	}

	while (treeCount < 50000) {
		spawnTree();
	}
	while (treeStumpCount < 3000) {
		spawnTreeStump();
	}

	while (rabbitCount < 300) {
		spawnRabbit();
	}
}

void SimpleRTSSimulation::update(Float timestep) {
	updateBases();
	updateHumans();
	updateRabbits();
}

#pragma once

#include "Game.hpp"
#include "Random.hpp"
#include "Math.hpp"
#include "Player.hpp"
#include "Heightmap.hpp"


#define SIMPLERTS_TREE_COLOR Color(0.0f, 0.9f, 0.0f, 1.0f)
#define SIMPLERTS_BARK_COLOR Color(0.107138f, 0.039452f, 0.01518f, 1.0f)
#define SIMPLERTS_RABBIT_COLOR Color(0.085714f, 0.046503f, 0.024848f, 1.0f)

#define SIMPLERTS_MAPSIZE 500

struct SimpleRTSSimulation : public GameSimulation {
	
	//terrain
	Float heightmap[SIMPLERTS_MAPSIZE][SIMPLERTS_MAPSIZE];
	CPUMesh terrainMesh;
	void generateTerrain() {
		
	};


	Float time = 0;
	Float timestep = 1.0f / 144.0f;

	enum class SimpleRTSItem {
		Nothing,
		Wood,
		Food
	};
	
	//trees
	UInt treeCount = 0;
	UInt lastTreeStump = 0;
	UInt treeStumpCount = 0;
	Vector<Float> treeAge;
	Vector<Float> treeSize;
	Vector<Vec2> treePosition;
	Vector<Vec2> treeDirection;
	Vector<Vec2> treeStumpPosition;
	void spawnTree() {
		treeAge.push_back(0.0f);
		treeSize.push_back(randomFloat(0.75,1.5));
		treeDirection.push_back(randomUnitVec2());
		treePosition.push_back(randomVec2(-SIMPLERTS_MAPSIZE, SIMPLERTS_MAPSIZE));
		treeCount++;
	}
	void spawnTreeStump() {
		treeStumpPosition.push_back(randomVec2(-SIMPLERTS_MAPSIZE, SIMPLERTS_MAPSIZE));
		treeStumpCount++;
	}
	void cutTree(Index id) {
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
	Index getCuttableTree(Vec2 position) {
		return idOfClosestPointInLoopingSpace(position, treePosition, SIMPLERTS_MAPSIZE);
	}

	//bases
	UInt baseCount = 0;
	Vector<Vec2> basePosition;
	Vector<UInt> baseClanID;
	void spawnBase(UInt clanID) {
		baseClanID.push_back(clanID);
		basePosition.push_back(randomVec2(-SIMPLERTS_MAPSIZE, SIMPLERTS_MAPSIZE));
		spawnHumanInBase(baseCount);
		baseCount++;
	}
	void produceWorker(UInt baseID) {
		if (clanFood[baseClanID[baseID]] > 0 && clanWood[baseClanID[baseID]] >= 5) {
			clanFood[baseClanID[baseID]]--;
			clanWood[baseClanID[baseID]] -= 5;;
			spawnHumanInBase(baseID);
		}
	};
	UInt getIndexOfClosestBaseOfClan(UInt clanID, Vec2 position) {
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
	};

	//humans
	enum class SimpleRTSHumanSex {
		Male,
		Female
	};
	enum class SimpleRTSHumanState {
		LookingForWood,
		Delivering,
		HuntingRabbit
	};
	UInt humanCount = 0;
	Float humanSpeed = 10;
	Vector<Index> humanClan;
	Vector<Index> humanTargetID;
	Vector<Vec2> humanPositions;
	Vector<Vec2> humanDirection;
	Vector<Vec2> humanTargetpositions;
	Vector<SimpleRTSHumanSex> humanSex;
	Vector<SimpleRTSItem> humanCarriedItem;
	Vector<SimpleRTSHumanState> humanState;

	void updateHumans() {
		for (UInt humanID = 0; humanID < humanCount; humanID++) {
			switch (humanState[humanID]) {
			case SimpleRTSHumanState::Delivering: deliverItem(humanID); break;
			case SimpleRTSHumanState::LookingForWood: chopWood(humanID); break;
			case SimpleRTSHumanState::HuntingRabbit: huntRabbit(humanID); break;
			default: break;
			}
		}
	}
	void chopWood(Index id) {
		if (humanPositions[id] != humanTargetpositions[id]) {
			moveHuman(id);
		}
		else {
			humanCarriedItem[id] = SimpleRTSItem::Wood;
			humanTargetpositions[id] = basePosition[getIndexOfClosestBaseOfClan(humanClan[id], humanPositions[id])];
			humanState[id] = SimpleRTSHumanState::Delivering;
		}
	};
	void moveHuman(Index id) {
		Vec2 delta = deltaInLoopingSpace(humanPositions[id], humanTargetpositions[id], SIMPLERTS_MAPSIZE);
		humanDirection[id] = normalize(delta);
		Vec2 step = humanDirection[id] * timestep * humanSpeed;
		if (length(delta) < length(step)) humanPositions[id] += delta;
		else humanPositions[id] += step;
		loopWithin(humanPositions[id], SIMPLERTS_MAPSIZE);
	}
	void huntRabbit(Index id) {
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
	void deliverItem(Index id) {
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
	};
	void spawnHumanInBase(Index baseID) {
		humanTargetID.push_back(0);

		humanClan.push_back(baseClanID[baseID]);
		humanDirection.push_back(randomUnitVec2());
		humanPositions.push_back(basePosition[baseID]);
		humanCarriedItem.push_back(SimpleRTSItem::Nothing);
		humanTargetpositions.push_back(basePosition[baseID]);
		humanState.push_back(SimpleRTSHumanState::LookingForWood);

		clanWorkerCount[baseClanID[baseID]]++;
		humanCount++;
	}

	//rabbits
	UInt rabbitCount = 0;
	Float rabbitJitter = 10;
	Float rabbitSpeed = humanSpeed * 0.8;
	Vector<Float> rabbitAge;
	Vector<Vec2> rabbitPosition;
	Vector<Vec2> rabbitVelocity;

	void spawnRabbit() {
		rabbitPosition.push_back(randomVec2(-SIMPLERTS_MAPSIZE, SIMPLERTS_MAPSIZE));
		rabbitVelocity.push_back(randomUnitVec2());
		rabbitCount++;
	}
	void updateRabbits() {
		for (Int i = 0; i < rabbitCount; i++) {
			rabbitVelocity[i] = normalize(rabbitVelocity[i] + rabbitJitter * timestep * randomUnitVec2());
			rabbitPosition[i] += rabbitSpeed * timestep * rabbitVelocity[i];
			loopWithin(rabbitPosition[i], SIMPLERTS_MAPSIZE);
		}
	}
	void killRabbit(Index id) {
		rabbitPosition[id] = randomVec2(-SIMPLERTS_MAPSIZE, SIMPLERTS_MAPSIZE);
		rabbitVelocity[id] = randomUnitVec2();
	}
	Index getClosestRabbit(Vec2 pos) {
		return idOfClosestPointInLoopingSpace(pos, rabbitPosition, SIMPLERTS_MAPSIZE);
	};

	//clans that humans belong to
	UInt clanCount = 0;
	Vector<UInt> clanFood;
	Vector<UInt> clanWood;
	Vector<UInt> clanWorkerCount;
	void spawnClan() {
		clanWorkerCount.push_back(0);
		clanFood.push_back(0);
		clanWood.push_back(0);
		spawnBase(clanCount);
		clanCount++;
	}
	void updateBases() {
		for (UInt baseID = 0; baseID < baseCount; baseID++) {
			produceWorker(baseID);
		}
	}
	SimpleRTSItem getLowestResource(Index clanID) {
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

	SimpleRTSSimulation() {
		generateTerrain();
		
		spawnClan();
		spawnClan();
		spawnClan();

		while (treeCount < 10000) {
			spawnTree();
		}
		while (treeStumpCount < 3000) {
			spawnTreeStump();
		}

		while (rabbitCount < 300) {
			spawnRabbit();
		}
	};
	void update(Float timestep) override {
		updateBases();
		updateHumans();
		updateRabbits();
	};
};

struct SimpleRTSRenderer : public GameRenderer {
	Mutex mutex;
	Player player;
	Action forward;
	Action backward;
	Action left;
	Action right;
	Action upward;
	Action downward;
	Float mouseSensitivity = 0.0015f;

	SimpleRTSSimulation* sim = nullptr;

	SimpleRTSRenderer(SimpleRTSSimulation& sim);
	void update(Renderer& renderer) override;
	void mouseIsMoving(Window& window, IVec2 position)  override;
	void render(TiledRectangle area, Renderer& renderer) override;
	void mouseIsScrolled(Window& window, Double xAxis, Double yAxis) override;
	void buttonIsPressed(Window& window, Key key, ActionState action, Int modifiers) override;
	void mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers) override;
};
#pragma once

#include "Game.hpp"
#include "Random.hpp"
#include "Math.hpp"
#include "Player.hpp"
#include "Heightmap.hpp"

#define SIMPLERTS_MAPSIZE 100

struct SimpleRTSSimulation;

namespace SimpleRTS {

enum class SimpleRTSItem {
	Nothing,
	Wood,
	Food
};

enum class SimpleRTSHumanSex {
	Male,
	Female
};

enum class SimpleRTSHumanState {
	LookingForWood,
	Delivering,
	HuntingRabbit,
};
struct RabbitSimulation {
	const Float rabbitJitter = 10;
	const Float rabbitSpeed = 8;
	
	UInt rabbitCount = 0;
	Vector<Float> rabbitAge;
	Vector<Vec2> rabbitPosition;
	Vector<Vec2> rabbitVelocity;

	void spawnRabbit();
	void updateRabbits();
	void killRabbit(Index id);
	Index getClosestRabbit(Vec2 pos);
};
struct HouseSimulation {
	UInt houseCount = 0;
	Vector<Vec2> housePosition;
	Vector<UInt> houseClanID;
};
struct BushSimulation {
	//bush
	Vector<Vec2> bushPosition;
	Vector<Vec2> bushDirection;

	//berry bush
	Vector<Vec2> berryBushPosition;
	Vector<Vec2> berryBushDirection;
};
struct TreeStumps {
	UInt lastTreeStump = 0;
	UInt treeStumpCount = 0;
	Vector<Vec2> treeStumpPosition;
};
struct Trees {
	const UInt maxTreeCount = SIMPLERTS_MAPSIZE * SIMPLERTS_MAPSIZE / 100;
	const UInt minTreeCount = 1000;
	const Float maxTreeLifeExpectancyInSeconds = 1 * 3;
	const Float minTreeLifeExpectancyInSeconds = 0 * 60;
	const Float fullyGrownMaxTreeSize = 5;
	const Float fullyGrownMinTreeSize = 2;
	UInt treeCount = 0;
	Vector<Float> treeAge;
	Vector<Float> treeSize;
	Vector<Vec2> treePosition;
	Vector<Vec2> treeDirection;
	Vector<Float> lifeExpectancy;

	Vector<Float> fullyGrownSize;

	Float calculateTreeSize(UInt id) {
		return fullyGrownSize[id] * (treeAge[id]/ lifeExpectancy[id]);
	}
	void update(SimpleRTSSimulation& simulation);
	void render(Renderer& renderer) {
		renderer.matrixSystem.matrixArray(treePosition, treeDirection, treeSize);
		renderer.renderMeshInstanced("Tree");
	}
	void create(SimpleRTSSimulation& simulation) {
		while (treeCount < minTreeCount) {
			spawnTree(simulation);
		}
	}
	void spawnTree(SimpleRTSSimulation& simulation) {
		lifeExpectancy.push_back(random(minTreeLifeExpectancyInSeconds, maxTreeLifeExpectancyInSeconds));
		treeAge.push_back(randomFloat(minTreeLifeExpectancyInSeconds, lifeExpectancy.back()));
		treeDirection.push_back(randomUnitVec2());
		treePosition.push_back(randomVec2(-SIMPLERTS_MAPSIZE, SIMPLERTS_MAPSIZE));
		fullyGrownSize.push_back(random(fullyGrownMinTreeSize, fullyGrownMaxTreeSize));
		treeSize.push_back(calculateTreeSize(treeCount));
		treeCount++;
	};
	void cutTree(UInt id) {
		treeSize[id] = 0;
		treeAge[id] = 0;
		Vec2 newTreePosition = treePosition[randomInt(treePosition.size())];
		loopWithin(newTreePosition, SIMPLERTS_MAPSIZE);
		while (isWithinDistanceOfOtherPoints(newTreePosition, treePosition, 1.0f)) {
			newTreePosition += normalize(randomVec2(-1, 1)) * 3.0f;
			loopWithin(newTreePosition, SIMPLERTS_MAPSIZE);
		}
		//spawn tree stump here
		//treeStumpPosition[lastTreeStump] = treePosition[id];
		//lastTreeStump++;
		//if (lastTreeStump >= treeStumpPosition.size()) lastTreeStump = 0;
		treePosition[id] = newTreePosition;
	}
};
struct HumanitySimulation {

	UInt humanCount = 0;
	Vector<Float> humanSpeed;
	Vector<Index> humanTargetID;
	Vector<Vec2> humanPositions;
	Vector<Vec2> humanDirection;
	Vector<Vec2> humanTargetpositions;
	Vector<SimpleRTSHumanSex> humanSex;
	Vector<SimpleRTSItem> humanCarriedItem;
	Vector<SimpleRTSHumanState> humanState;

	void spawnHuman() {

	};
	void spawnHuman(Vec2 position) {
		humanTargetID.push_back(0);
		humanSpeed.push_back(randomFloat(8.1, 15));
		humanDirection.push_back(randomUnitVec2());
		humanPositions.push_back(position);
		humanCarriedItem.push_back(SimpleRTSItem::Nothing);
		humanTargetpositions.push_back(position);
		humanState.push_back(SimpleRTSHumanState::LookingForWood);
		humanCount++;
	}
	void updateHumans();
	void chopWood(Index id);
	void moveHuman(Index id);
	void huntRabbit(Index id);
	void deliverItem(Index id);
};
}

struct SimpleRTSSimulation : public GameSimulation {
	Float time = 0;
	Float timestep = 1.0f / 144.0f;

	Float dimensions = 200;

	SimpleRTS::Trees trees;
	SimpleRTS::HumanitySimulation humanitySimulation;
	SimpleRTS::BushSimulation bushSimulation;
	SimpleRTS::HouseSimulation houseSimulation;
	SimpleRTS::RabbitSimulation rabbitSimulation;

	SimpleRTSSimulation();
	void update(Float timestep) override;
	void render(Renderer& renderer) {
		trees.render(renderer);
	};
};

struct SimpleRTSRenderer : public GameRenderer {
	Mutex mutex;
	DebugPlayer player;
	SimpleRTSSimulation* sim = nullptr;

	//Input
	InputEvent exit = InputEvent(InputType::Mouse, RMB, 0);
	InputEvent enter = InputEvent(InputType::Mouse, LMB, 1);

	SimpleRTSRenderer(SimpleRTSSimulation& sim);
	void create(Window& window) {};
	void update(Window& window) override;
	void render(Window& window, TiledRectangle area) override;
	void inputEvent(Window& window, InputEvent input) override;
	void mouseMoved(Window& window, MouseMovementInput input) override;
};
#pragma once

#include "Game.hpp"
#include "Random.hpp"
#include "Math.hpp"
#include "Player.hpp"
#include "Heightmap.hpp"

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
	void spawnTree();
	void spawnTreeStump();
	void cutTree(Index id);
	Index getCuttableTree(Vec2 position);

	//bush
	Vector<Vec2> bushPosition;
	Vector<Vec2> bushDirection;

	//berry bush
	Vector<Vec2> berryBushPosition;
	Vector<Vec2> berryBushDirection;

	//bases
	UInt baseCount = 0;
	Vector<Vec2> basePosition;
	Vector<UInt> baseClanID;
	void spawnBase(UInt clanID);
	void produceWorker(UInt baseID);;
	UInt getIndexOfClosestBaseOfClan(UInt clanID, Vec2 position);;

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
	Vector<Float> humanSpeed;
	Vector<Index> humanClan;
	Vector<Index> humanTargetID;
	Vector<Vec2> humanPositions;
	Vector<Vec2> humanDirection;
	Vector<Vec2> humanTargetpositions;
	Vector<SimpleRTSHumanSex> humanSex;
	Vector<SimpleRTSItem> humanCarriedItem;
	Vector<SimpleRTSHumanState> humanState;

	void updateHumans();
	void chopWood(Index id);;
	void moveHuman(Index id);
	void huntRabbit(Index id);
	void deliverItem(Index id);;
	void spawnHumanInBase(Index baseID);

	//rabbits
	UInt rabbitCount = 0;
	Float rabbitJitter = 10;
	Float rabbitSpeed = 8;
	Vector<Float> rabbitAge;
	Vector<Vec2> rabbitPosition;
	Vector<Vec2> rabbitVelocity;

	void spawnRabbit();
	void updateRabbits();
	void killRabbit(Index id);
	Index getClosestRabbit(Vec2 pos);

	//clans that humans belong to
	UInt clanCount = 0;
	Vector<UInt> clanFood;
	Vector<UInt> clanWood;
	Vector<UInt> clanWorkerCount;
	void spawnClan();
	void updateBases();
	SimpleRTSItem getLowestResource(Index clanID);

	SimpleRTSSimulation();;
	void update(Float timestep) override;;
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
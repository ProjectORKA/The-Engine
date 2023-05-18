#pragma once

#include "Game.hpp"
#include "Random.hpp"
#include "Physics.hpp"
#include "Collision.hpp"

constexpr UInt nodeCount = 500;
constexpr Float particleSize = 0.00;
constexpr Float lineSize = 0.002f;
constexpr Float startingVelocity = 0.0;
constexpr Vec2 gravity = Vec2(0, 0);
constexpr Float connectionDistance = 0.1f;

struct PhysicsParticle {
	Vec2 position = randomVec2(-1, 1);
	Vec2 velocity = randomVec2(-startingVelocity, startingVelocity);
	Bool isColliding = false;
	Vector<Index> connected = {};

	void render(ResourceManager& resourceManager, Window& window, Vector<PhysicsParticle>& particles) const;
};

struct PhysicsPlaygroundConnection {
	Index a = -1;
	Index b = -1;

	PhysicsPlaygroundConnection(Index a, Index b);
};

struct PhysicsPlaygroundSimulation : public GameSimulation {
	Bool paused = true;
	Vector<PhysicsParticle> nodes;
	Vector<PhysicsPlaygroundConnection> connections;

	PhysicsPlaygroundSimulation();
	Vec2 getPointOnSmoothCurve(Float value) const;

	void connectByDistance();
	void connectAllNodesInALine();
	void neighborBasedConnections();
	void removeIntersecting();
	void connectNeighbors(Index node);
	void shorterFullConnection();
	void intersectionBasedConnections();
	void makeConnection(Index a, Index b);
	void connectNeighborOfNeighbortoSelf();
	void removeConnection(Index a, Index b);
	void addOneWayConnection(Index aid, Index bid);
	Float distanceBetweenTwoNodes(Index a, Index b) const;
	static Bool doIntersect(Vec2 a, Vec2 b, Vec2 c, Vec2 d);
	Bool doIntersect(Index a, Index b, Index c, Index d) const;
	Float lengthOfEntireRun() const;
	void swapNodesToShortest();
	void swapIntersecting();
	void start(ResourceManager& resourceManager) override;
	void update(Float delta) override;
};

struct PhysicsPlayGroundRenderer : public GameRenderer {
	PhysicsPlaygroundSimulation* sim = nullptr;

	InputEvent pause = InputEvent(InputType::KeyBoard, P, false);

	void inputEvent(Window& window, InputEvent input) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct PhysicsPlayground {
	UserInterface ui;
	ResourceManager resourceManager;
	PhysicsPlaygroundSimulation sim;

	PhysicsPlayGroundRenderer game;

	void run() {
		sim.start(resourceManager);

		game.sim = &sim;
		ui.window("Physics playground", 1024, true, WindowState::windowed, game, resourceManager);
		ui.run();

		//sim.stop();
	}
};

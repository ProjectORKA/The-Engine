#pragma once

#include "Game.hpp"
#include "Random.hpp"

constexpr UInt  nodeCount          = 128;
constexpr Float startingVelocity   = 0.0f;
constexpr Float connectionDistance = 0.1f;
constexpr Float particleSize       = 0.00;
constexpr Float lineSize           = 0.002f;
constexpr Vec2  gravity            = Vec2(0, 0);

struct PhysicsParticle
{
	Vector<Index> connected   = {};
	Bool          isColliding = false;
	Vec2          position    = randomVec2Fast(-1, 1);
	Vec2          velocity    = randomVec2Fast(-startingVelocity, startingVelocity);

	void render(Window& window, Vector<PhysicsParticle>& particles) const;
};

struct PhysicsPlaygroundConnection
{
	Index a = -1;
	Index b = -1;

	PhysicsPlaygroundConnection(Index a, Index b);
};

struct PhysicsPlaygroundSimulation : GameSimulation
{
	Vector<PhysicsParticle>             nodes;
	Vector<PhysicsPlaygroundConnection> connections;
	Bool                                paused = true;

	void swapIntersecting();
	void destroy() override;
	void connectByDistance();
	void removeIntersecting();
	void swapNodesToShortest();
	void shorterFullConnection();
	void connectAllNodesInALine();
	void neighborBasedConnections();
	void connectNeighbors(Index node);
	void update(Float delta) override;
	void intersectionBasedConnections();
	void makeConnection(Index a, Index b);
	void connectNeighborOfNeighborToSelf();
	void removeConnection(Index a, Index b);
	void addOneWayConnection(Index aid, Index bid);
	void create() override;

	[[nodiscard]] Float lengthOfEntireRun() const;
	[[nodiscard]] Vec2  getPointOnSmoothCurve(Float value) const;
	[[nodiscard]] Bool  doIntersect(Vec2 a, Vec2 b, Vec2 c, Vec2 d) const;
	[[nodiscard]] Float distanceBetweenTwoNodes(Index a, Index b) const;
	[[nodiscard]] Bool  doIntersect(Index a, Index b, Index c, Index d) const;
};

struct PhysicsPlayGroundRenderer : GameRenderer
{
	const Bool                   smoothCurve = false;
	PhysicsPlaygroundSimulation* sim         = nullptr;
	InputEvent                   pause       = InputEvent(InputType::KeyBoard, SPACE, false);

	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(Window& window) override;
	void render(Window& window, TiledRectangle area) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct PhysicsPlayground
{
	UserInterface               ui;
	PhysicsPlaygroundSimulation sim;
	Window                      window;
	PhysicsPlayGroundRenderer   renderer;

	void run()
	{
		ui.create();
		sim.start();
		renderer.connect(sim);
		ui.window("Physics Playground", Area(1024, 1024), true, true, WindowState::Windowed, renderer);
		ui.run();
		sim.stop();
	}
};

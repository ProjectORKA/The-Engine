#pragma once

#include "Game.hpp"
#include "Random.hpp"

constexpr UInt  nodeCount          = 128;
constexpr Float startingVelocity   = 0.0f;
constexpr Float connectionDistance = 0.1f;
constexpr Float particleSize       = 0.02f;
constexpr Float lineSize           = 0.002f;
constexpr Vec2  gravity            = Vec2(0, -1.0f);

struct PhysicsParticle
{
	Bool          isColliding = false;
	Vec2          position    = randomVec2Fast(-1, 1);
	Vec2          velocity    = randomVec2Fast(-startingVelocity, startingVelocity);
};

struct PhysicsPlaygroundSimulation : GameSimulation
{
	Vector<PhysicsParticle>             particles;
	Bool                                paused = true;

	void create() override;
	void destroy() override;
	void update(Float delta) override;

	[[nodiscard]] Float distanceBetweenTwoNodes(Index a, Index b) const;
};

struct PhysicsPlayGroundRenderer : GameRenderer
{
	PhysicsPlaygroundSimulation* sim         = nullptr;
	InputEvent                   pause       = InputEvent(InputType::KeyBoard, Space, false);

	void update(Window& window) override;
	void create(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void render(Window& window, TiledRectangle area) override;
	void inputEvent(Window& window, InputEvent input) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct PhysicsPlayground
{
	PhysicsPlaygroundSimulation sim;
	PhysicsPlayGroundRenderer   renderer;

	void run()
	{
		sim.start();
		renderer.connect(sim);
		ui.window("Physics Playground", Area(1024, 1024), true, true, WindowState::Windowed, renderer);
		ui.run();
		sim.stop();
	}
};

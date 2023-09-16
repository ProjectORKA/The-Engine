#pragma once
#include "Game.hpp"
#include "PongBall.hpp"
#include "PongPlayer.hpp"

struct Renderer;
struct Window;

struct PongRenderer final : GameRenderer
{
	Vector<PongBall> balls;
	PongPlayer   players[2];
	UInt         ballCount             = 10;

	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(ResourceManager& resourceManager, Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct Pong
{
	UserInterface   ui;
	PongRenderer    pongRenderer;
	ResourceManager resourceManager;

	void run();
};



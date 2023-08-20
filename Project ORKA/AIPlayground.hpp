#pragma once

#include "Game.hpp"
#include "NeuralNetwork.hpp"

struct AIPlaygroundRenderer final : GameRenderer
{
	Framebuffer framebuffer;
	NeuralNet   network = NeuralNet({9, 15, 16, 5, 3});

	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(ResourceManager& resourceManager, Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct AIPlayground
{
	UserInterface        ui;
	ResourceManager      resourceManager;
	AIPlaygroundRenderer sandboxRenderer;

	void run();
};

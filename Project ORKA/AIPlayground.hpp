#pragma once

#include "Game.hpp"
#include "NeuralNetwork.hpp"
#include "Player.hpp"

struct TerrainZoomNeuralNet : NeuralNet
{
	void create()
	{
		Vector<ULL> input;
		for(Int y = 0; y < 10; y++) input.emplace_back(y);

		for(Int x = 0; x < 10; x++)
		{
			for(Int y = 0; y < 10; y++)
			{
				addNeuron(Vec3(x, y, 0), input, {});
			}
		}
	}
};

struct AIPlaygroundRenderer final : GameRenderer
{
	DebugPlayer          player;
	UIContainer          topBar;
	TerrainZoomNeuralNet network;
	Framebuffer          framebuffer;

	InputEvent enter           = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent exit            = InputEvent(InputType::Mouse, Rmb, false);
	InputEvent wireframeToggle = InputEvent(InputType::KeyBoard, F, true);

	void update(Window& window) override;
	void create(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void render(Window& window, TiledRectangle area) override;
	void inputEvent(Window& window, InputEvent input) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct AIPlayground
{
	AIPlaygroundRenderer renderer;

	void run()
	{
		ui.window("ORKA AI Playground", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};

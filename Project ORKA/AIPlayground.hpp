#pragma once

#include "Game.hpp"
#include "NeuralNetwork.hpp"
#include "Player.hpp"

struct TerrainZoomNeuralNet : NeuralNet
{
	void create()
	{
		for(Int x = 0; x < 128; x++)
		{
			for(Int y = 0; y < 128; y++)
			{
				addNeuron(Vec3(x, y, 0), {}, {});
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

	InputEvent enter           = InputEvent(InputType::Mouse, LMB, true);
	InputEvent exit            = InputEvent(InputType::Mouse, RMB, false);
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
	UserInterface        ui;
	Window               window;
	AIPlaygroundRenderer renderer;

	void run()
	{
		ui.create();
		ui.window("ORKA AI Playground", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};

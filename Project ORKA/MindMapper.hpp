#pragma once

#include "Game.hpp"
#include "JobSystem.hpp"
#include "MindMap.hpp"
#include "Player.hpp"

void simWork(MindMap& system);

struct MindMapperSim final : GameSimulation
{
	MindMap   system;
	const Int nodeCount = 100;

	void destroy() override;
	void update(Float delta) override;
	void create() override;
};

struct MindMapperRenderer final : GameRenderer
{
	DebugPlayer    player;
	Bool           grow       = false;
	MindMapperSim* sim        = nullptr;
	InputEvent     enter      = InputEvent(InputType::Mouse, 0, true);
	InputEvent     exit       = InputEvent(InputType::Mouse, 1, false);
	InputEvent     growToggle = InputEvent(InputType::KeyBoard, Space, false);

	void update(Window& window) override;
	void create(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void render(Window& window, TiledRectangle area) override;
	void inputEvent(Window& window, InputEvent input) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct MindMapper
{
	UserInterface      ui;
	MindMapperSim      sim;
	Window             window;
	MindMapperRenderer renderer;

	void run()
	{
		ui.create();
		sim.start();
		renderer.connect(sim);
		ui.window("ORKA MindMapper", Area(1920, 1080), true, true, WindowState::Windowed, renderer);
		ui.run();
		sim.stop();
	}
};

#pragma once

#include "Game.hpp"
#include "Basics.hpp"
#include "Player.hpp"
#include "UIButton.hpp"

struct Renderer;
struct Window;

struct Toggle final : UIButton
{
	Bool toggle = false;

	void doThis() override;
	void update(Window& window) override;
	void destroy(Window& window) override;
	void create(Window& window) override;
};

struct UIBucketGraph : UIElement
{
	void update(Window& window) override {}

	void destroy(Window& window) override {}

	void inputEvent(Window& window, InputEvent input) override {}

	void create(Window& window) override {}

	void render(Window& window, TiledRectangle area) override {}

	void renderInteractive(Window& window, TiledRectangle area) override {}
};

struct UISandboxRenderer final : GameRenderer
{
	Vector<Int> values;

	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(Window& window) override;
	void render(Window& window, TiledRectangle area) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct UISandbox
{
	UserInterface     ui;
	Window            window;
	UISandboxRenderer renderer;

	void run()
	{
		ui.create();
		ui.window("ORKA UI Sandbox", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};

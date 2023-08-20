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
	void create(ResourceManager& resourceManager, Window& window) override;
};

struct UISandboxRenderer final : GameRenderer
{
	UIButton saveButton;
	Toggle   pauseButton;
	Bool     paused = false;

	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(ResourceManager& resourceManager, Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct UISandbox
{
	UserInterface     ui;
	UISandboxRenderer renderer;
	ResourceManager   resourceManager;

	void run();
};

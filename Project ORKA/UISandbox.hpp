#pragma once

#include "Game.hpp"
#include "Basics.hpp"
#include "Player.hpp"
#include "Random.hpp"
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

struct UISandbox
{
	void run()
	{
		UIContainer global;
		//UIContainer top;
		//UIContainer main;
		//UIContainer side;
		//UIContainer page;

		global.name("GlobalView");
		global.fill(Color(0,0,1,1));
		global.vertical();
		//global.add(top);
		//global.add(main);

		//top.name("TopBar");
		//top.fill(Color(1,0,0,1));
		//top.horizontal();

		//main.name("MainView");
		//main.horizontal();
		//main.add(side);
		//main.add(page);
		//main.fill(randomColorFast());

		//side.name("SideView");
		//side.fill(randomColorFast());
		//side.vertical();

		//page.name("MainPage");
		//page.fill(randomColorFast());
		//page.vertical();

		Window& window = ui.window("ORKA UI Sandbox", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, global);
		window.renderer.setWireframeMode(true);
		ui.run();
	}
};

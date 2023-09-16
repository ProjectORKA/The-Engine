
#include "Game.hpp"


struct ORKAJumpRenderer : GameRenderer {

	void update(Window& window){}
	void destroy(Window& window) {}
	void connect(GameSimulation& simulation) {}
	void inputEvent(Window& window, InputEvent input) {}
	void create(ResourceManager& resourceManager, Window& window) {}
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) {
		Renderer& r = window.renderer;

		r.clearBackground(Color(1));


	}
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {}
};

struct ORKAJump {
	UserInterface ui;
	ORKAJumpRenderer renderer;
	ResourceManager resourceManager;

	void run() {
		resourceManager.create();
		ui.create();
		ui.window("ORKA Jump", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, renderer, resourceManager);
		ui.run();
	}
};
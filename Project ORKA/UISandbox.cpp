#include "UISandbox.hpp"

#include "Window.hpp"

#include "Random.hpp"

UISandbox::UISandbox() { pauseButton.padding(50); }

void UISandbox::update(Window& window) {
	pauseButton.update(window);
	saveButton.update(window);
}

void UISandbox::render(ResourceManager& resourceManager, Window& window, TiledRectangle area) {
	Renderer& renderer = window.renderer;

	renderer.clearColor(Color(0.008, 0.008, 0.009, 1));

	pauseButton.render(resourceManager, window, area);

	if (paused) {
		area.size.x /= 2;
		area.size.y /= 2;
		area.position.x + 10;
		area.position.y + 10;

		saveButton.render(resourceManager, window, area);
	}
}

void UISandbox::inputEvent(Window& window, const InputEvent input) {
	saveButton.inputEvent(window, input);
	pauseButton.inputEvent(window, input);

	paused = pauseButton.toggle;
}

void UISandbox::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {
	Renderer& renderer = window.renderer;

	pauseButton.renderInteractive(resourceManager, window, area);

	if (paused) {
		area.size.x /= 2;
		area.size.y /= 2;
		area.position.x + 10;
		area.position.y + 10;

		saveButton.renderInteractive(resourceManager, window, area);
	}
}

void Toggle::update(Window& window) { content->update(window); }

void Toggle::doThis() { toggle = !toggle; }


#include "UISandbox.hpp"

#include "Window.hpp"

#include "Random.hpp"

UISandbox::UISandbox() {
	pauseButton.padding(50);
}
void UISandbox::render(Window& window, TiledRectangle area) {
	Renderer& renderer = window.renderer;
	
	renderer.clearColor(Color(0.008, 0.008, 0.009, 1));

	pauseButton.render(window, area);

	if (paused) {
		area.size.x /= 2;
		area.size.y /= 2;
		area.position.x + 10;
		area.position.y + 10;

		saveButton.render(window, area);
	}

}
void UISandbox::inputEvent(Window& window, InputEvent input) {
	saveButton.inputEvent(window, input);
	pauseButton.inputEvent(window, input);
	
	paused = pauseButton.toggle;
}
void UISandbox::mouseMoved(Window& window, MouseMovementInput input) {
	pauseButton.mouseMoved(window, input);
	saveButton.mouseMoved(window, input);
}
void UISandbox::renderInteractive(Window& window, TiledRectangle area) {
	Renderer& renderer = window.renderer;

	pauseButton.renderInteractive(window, area);
	
	if (paused) {
		area.size.x /= 2;
		area.size.y /= 2;
		area.position.x + 10;
		area.position.y + 10;

		saveButton.renderInteractive(window, area);
	}
}
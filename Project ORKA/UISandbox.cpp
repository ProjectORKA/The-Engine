
#include "UISandbox.hpp"

#include "Window.hpp"

#include "Random.hpp"


void UISandbox::render(TiledRectangle area, Renderer& renderer) {
	renderer.clearColor(Color(0.008, 0.008, 0.009, 1));

	pauseButton.render(area,renderer);

	if (paused) {
		area.size.x /= 2;
		area.size.y /= 2;
		area.position.x + 10;
		area.position.y + 10;

		saveButton.render(area, renderer);
	}

}
void UISandbox::renderInteractive(TiledRectangle area, Renderer& renderer) {
	
	pauseButton.renderInteractive(area, renderer);
	
	if (paused) {
		area.size.x /= 2;
		area.size.y /= 2;
		area.position.x + 10;
		area.position.y + 10;

		saveButton.renderInteractive(area, renderer);
	}
}
void UISandbox::mouseIsMoving(Window& window, IVec2 position) {
	pauseButton.mouseIsMoving(window, position);
	saveButton.mouseIsMoving(window, position);
}
void UISandbox::mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers) {
	saveButton.mouseIsPressed(window, button, action, modifiers);
	pauseButton.mouseIsPressed(window, button, action, modifiers);
	
	paused = pauseButton.toggle;
}
void UISandbox::buttonIsPressed(Window& window, Key key, ActionState action, Int modifiers) {
	saveButton.buttonIsPressed(window, key, action, modifiers);
	pauseButton.buttonIsPressed(window, key, action, modifiers);
}

#include "UIButton.hpp"
#include "Window.hpp"

UIButton::UIButton() {
}
UIButton& UIButton::insert(UIElement& element) {
	content = &element;
	return *this;
}
void UIButton::render(TiledRectangle renderArea, Renderer& renderer) {

	constraints.update(renderArea);

	renderer.useShader("color");
	if (pressed) {
		renderer.uniforms().customColor(Color(1, 1, 0, 1));
	}
	else {
		PixelIDs ids = renderer.getIDsUnderCursor();
		if (ids.objectID == id) {
			renderer.uniforms().customColor(Color(1));
		}
		else {
			renderer.uniforms().customColor(Color(0.1, 0.1, 0.1, 1));
		}
	}

	renderer.screenSpace();
	renderer.uniforms().mMatrix(matrixFromTiledRectangle(renderArea));
	renderer.renderMesh("button");

	if (content) content->render(renderArea, renderer);
}
void UIButton::renderInteractive(TiledRectangle renderArea, Renderer& renderer) {
	
	constraints.update(renderArea);
	
	renderer.useShader("idShader");
	renderer.screenSpace();
	renderer.uniforms().objectID(id);
	renderer.uniforms().mMatrix(matrixFromTiledRectangle(renderArea));

	renderer.renderMesh("button");

}
void UIButton::mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers) {
	if (button == MouseButton::L) {
		if (action == ActionState::Press) pressed = window.renderer.framebufferSystem.idFramebuffer().currentIds.objectID == id;
		else {
			if (window.renderer.framebufferSystem.idFramebuffer().currentIds.objectID == id) {
				if (pressed) {
					pressed = false;
					doThis();
				}
			}
			else pressed = false;
		}
	}
	else pressed = false;
}
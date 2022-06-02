
#include "UIButton.hpp"
#include "Window.hpp"

UIButton::UIButton() {

}
UIButton& UIButton::insert(UIElement& element) {
	content = &element;
	return *this;
}
void UIButton::render(Window& window, TiledRectangle renderArea) {
	Renderer& renderer = window.renderer;

	constraints.update(renderArea);

	renderer.useShader("color");
	if (pressed) {
		renderer.uniforms().customColor(Color(1, 1, 0, 1));
	}
	else {
		UInt objectID = renderer.idFramebuffer.objectID;

		if (objectID == id) {
			renderer.uniforms().customColor(Color(1));
		}
		else {
			renderer.uniforms().customColor(Color(0.1, 0.1, 0.1, 1));
		}
	}

	renderer.screenSpace();
	renderer.uniforms().mMatrix(matrixFromTiledRectangle(renderArea));
	renderer.renderMesh("plane");

	if (content) content->render(window, renderArea);
}
void UIButton::renderInteractive(Window& window, TiledRectangle renderArea) {
	
	constraints.update(renderArea);
	
	window.renderer.useShader("idShader");
	window.renderer.screenSpace();
	window.renderer.uniforms().objectID(id);
	window.renderer.uniforms().mMatrix(matrixFromTiledRectangle(renderArea));

	window.renderer.renderMesh("plane");
}

void UIButton::inputEvent(Window& window, InputEvent input)
{
	if (input == InputEvent(InputType::Mouse,0,1)) pressed = window.renderer.idFramebuffer.objectID == id;
	if (input == InputEvent(InputType::Mouse, 0, 0)) {
		if (pressed) {
			pressed = false;
			doThis();
		}
	}

	if(content)content->inputEvent(window, input);
}
#include "UIButton.hpp"
#include "Window.hpp"

UIButton::UIButton() = default;

void UIButton::doThis()
{
	beep();
}

void UIButton::destroy(Window& window) {}

void UIButton::update(Window& window)
{
	if (content) content->update(window);
}

UIButton& UIButton::insert(UIElement& element)
{
	content = &element;
	return *this;
}

void UIButton::create(Window& window) {}

void UIButton::inputEvent(Window& window, const InputEvent input)
{
	if (input == InputEvent(InputType::Mouse, 0, true)) pressed = window.renderer.objectId == getId();
	if (input == InputEvent(InputType::Mouse, 0, false))
	{
		if (pressed)
		{
			pressed = false;
			doThis();
		}
	}

	if (content) content->inputEvent(window, input);
}

void UIButton::render(Window& window)
{
	Renderer& renderer = window.renderer;
	const IVec2 renderPos = getPosition() + IVec2(0, window.getHeight());

	renderer.useShader("color");
	if (pressed) renderer.uniforms().setCustomColor(Color(1, 1, 0, 1));
	else
	{
		if (const UInt objectId = renderer.objectId; objectId == getId()) renderer.uniforms().setCustomColor(Color(1));
		else renderer.uniforms().setCustomColor(Color(0.1, 0.1, 0.1, 1));
	}

	renderer.screenSpaceFromBottomLeft();
	renderer.uniforms().setMMatrix(matrixFromPositionAndSize(renderPos, getSize()));
	renderer.rectangle();

	if (content) content->render(window);
}

void UIButton::renderInteractive(Window& window)
{
	const IVec2 renderPos = getPosition() + IVec2(0, window.getHeight());
	window.renderer.useShader("idShader");
	window.renderer.screenSpaceFromBottomLeft();
	window.renderer.uniforms().setObjectId(getId());
	window.renderer.uniforms().setMMatrix(matrixFromPositionAndSize(renderPos, getSize()));
	window.renderer.renderMesh("plane");
}

#include "UIButton.hpp"
#include "Renderer.hpp"
#include "Window.hpp"

UIButton::UIButton() = default;

void UIButton::doThis()
{}

void UIButton::destroy(Window& window) {}

UIButton& UIButton::insert(UIElement& element)
{
	content = &element;
	addElement(element);
	return *this;
}

void UIButton::create(Window& window) {}

void UIButton::inputEvent(Window& window, InputEvent& input)
{
	if (input == InputEvent(InputType::Mouse, 0, true))
	{
		const Bool inside = hovered || isPointInside(window, window.mousePositionBotLeft());
		pressed          = inside;
		active           = pressed;
		if (pressed) input.consume();
	}
	if (input == InputEvent(InputType::Mouse, 0, false))
	{
		const Bool inside = hovered || isPointInside(window, window.mousePositionBotLeft());
		if (pressed && inside)
		{
			doThis();
			input.consume();
		}
		pressed = false;
		active  = false;
	}
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

void UIButton::onMouseEnter(Window& /*window*/)
{
	hovered = true;
}

void UIButton::onMouseLeave(Window& /*window*/)
{
	hovered = false;
	pressed = false;
	active  = false;
}

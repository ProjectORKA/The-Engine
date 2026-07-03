#include "UIButton.hpp"
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
		const Bool inside = hovered || isPointInside(window, window.mousePosBotLeft);
		pressed          = inside;
		if (pressed) input.consume();
	}
	if (input == InputEvent(InputType::Mouse, 0, false))
	{
		const Bool inside = hovered || isPointInside(window, window.mousePosBotLeft);
		if (pressed && inside)
		{
			doThis();
			input.consume();
		}
		pressed = false;
	}
}

//void UIButton::render(Window& window)
//{
//	Renderer& renderer = window.renderer;
//	const IVec2 renderPos  = getPosition() + IVec2(0, window.getHeight());
//	const IVec2 renderSize = getSize();
//	const Bool  hoveredNow = hovered || isPointInside(window, window.mousePosBotLeft);
//
//	renderer.useShader("color");
//	if (pressed) renderer.uniforms().setCustomColor(Color(1, 1, 0, 1));
//	else if (hasHover && hoveredNow) renderer.uniforms().setCustomColor(getHoverColor());
//	else if (const UInt objectId = renderer.objectId; objectId == getId()) renderer.uniforms().setCustomColor(Color(1));
//	else renderer.uniforms().setCustomColor(Color(0.1, 0.1, 0.1, 1));
//
//	renderer.screenSpaceFromBottomLeft();
//	renderer.uniforms().setMMatrix(matrixFromPositionAndSize(renderPos, renderSize));
//	renderer.rectangle();
//
//	renderBorder(renderer, renderPos, renderSize, [&](const IVec2& pos, const IVec2& size) {
//		renderer.uniforms().setMMatrix(matrixFromPositionAndSize(pos, size));
//		renderer.rectangle();
//	});
//
//	if (content) content->render(window);
//}

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
}

#include "UISlider.hpp"

#include "Window.hpp"

namespace
{
	constexpr Int sliderHorizontalPadding = 12;
	constexpr Int sliderVerticalPadding   = 12;
	constexpr Int sliderTrackHeight       = 8;
	constexpr Int sliderThumbWidth        = 10;
}

UISlider::UISlider()
{
	growX(true);
	growY(false);
	setHeight(58);
	background(Color(0.08f, 0.08f, 0.08f, 0.85f));
	hoveredBackground(Color(0.12f, 0.12f, 0.12f, 0.90f));
	activeBackground(Color(0.18f, 0.18f, 0.18f, 0.95f));
	border(1, Color(1, 1, 1, 0.15f));
}

UISlider& UISlider::setLabel(const String& label)
{
	labelText = label;
	return *this;
}

UISlider& UISlider::setRange(const Float newMinValue, const Float newMaxValue)
{
	minValue = min(newMinValue, newMaxValue);
	maxValue = max(newMinValue, newMaxValue);
	commitValue(getValue());
	return *this;
}

UISlider& UISlider::bind(Float& valueRef)
{
	boundValue = &valueRef;
	currentValue = valueRef;
	commitValue(currentValue);
	return *this;
}

UISlider& UISlider::setValue(const Float value)
{
	commitValue(value);
	return *this;
}

UISlider& UISlider::setOnValueChanged(Function<void(Float)> callback)
{
	onValueChanged = std::move(callback);
	return *this;
}

Float UISlider::getValue() const
{
	if(boundValue) return *boundValue;
	return currentValue;
}

void UISlider::syncBoundValue()
{
	if(boundValue) *boundValue = currentValue;
}

Float UISlider::normalizedValue() const
{
	const Float span = max(maxValue - minValue, 0.0001f);
	return clamp((getValue() - minValue) / span, 0.0f, 1.0f);
}

void UISlider::commitValue(const Float value)
{
	const Float clampedValue = clamp(value, minValue, maxValue);
	const Float previous = currentValue;
	currentValue = clampedValue;
	syncBoundValue();
	if(onValueChanged && abs(previous - clampedValue) > 0.000001f) onValueChanged(clampedValue);
}

void UISlider::setFromMouse(Window& window)
{
	const DVec2 localMouse = getMousePosBottomLeft(window);
	const Float usableWidth = static_cast<Float>(max(getWidth() - sliderHorizontalPadding * 2, 1));
	const Float value01 = clamp(static_cast<Float>(localMouse.x - sliderHorizontalPadding) / usableWidth, 0.0f, 1.0f);
	const Float newValue = minValue + (maxValue - minValue) * value01;
	commitValue(newValue);
}

void UISlider::update(Window& window)
{
	if(dragging)
	{
		setFromMouse(window);
		if(!window.pressed(InputId(InputType::Mouse, 0))) dragging = false;
	}

	UIElement::update(window);
}

void UISlider::render(Window& window)
{
	UIElement::render(window);

	Renderer& renderer = window.renderer;
	const IVec2 basePos = getPosition() + IVec2(0, window.getHeight());

	const Int trackWidth = max(getWidth() - sliderHorizontalPadding * 2, 1);
	const Int trackX = basePos.x + sliderHorizontalPadding;
	const Int trackY = basePos.y - getHeight() + sliderVerticalPadding + 4;

	const Float value01 = normalizedValue();
	const Int filledWidth = static_cast<Int>(value01 * static_cast<Float>(trackWidth));
	const Int thumbCenterX = trackX + filledWidth;

	renderer.useShader("color");
	renderer.uiSpace(window);

	renderer.fill(0.20f, 0.20f, 0.20f, 1.0f);
	renderer.UIRectangle(IVec2(trackX, trackY + sliderTrackHeight), IVec2(trackWidth, sliderTrackHeight));

	renderer.fill(0.65f, 0.65f, 0.65f, 1.0f);
	renderer.UIRectangle(IVec2(trackX, trackY + sliderTrackHeight), IVec2(max(filledWidth, 1), sliderTrackHeight));

	const Int thumbX = clamp(thumbCenterX - sliderThumbWidth / 2, trackX, trackX + trackWidth - sliderThumbWidth);
	renderer.fill(dragging ? Color(0.92f, 0.92f, 0.92f, 1.0f) : Color(0.85f, 0.85f, 0.85f, 1.0f));
	renderer.UIRectangle(IVec2(thumbX, trackY + sliderTrackHeight + 6), IVec2(sliderThumbWidth, sliderTrackHeight + 10));

	renderer.useShader("text");
	renderer.uniforms().setMMatrix(Matrix(1));
	renderer.fontSize(14.0f);
	renderer.letterSpacing(0.45f);
	renderer.fill(1, 1, 1, 1);
	renderer.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
	const String sliderText = showValueText ? labelText + ": " + toString(getValue()) : labelText;
	renderer.text(sliderText, Vec2(static_cast<Float>(basePos.x + sliderHorizontalPadding), static_cast<Float>(basePos.y - 18)));
}

void UISlider::inputEvent(Window& window, InputEvent& input)
{
	if(input == InputEvent(InputType::Mouse, 0, true))
	{
		const Bool inside = hovered || isPointInside(window, window.mousePosBotLeft);
		if(inside)
		{
			dragging = true;
			active = true;
			setFromMouse(window);
			input.consume();
		}
	}

	if(input == InputEvent(InputType::Mouse, 0, false))
	{
		if(dragging)
		{
			dragging = false;
			active = false;
			input.consume();
		}
	}
}

void UISlider::onMouseEnter(Window& window)
{
	UIElement::onMouseEnter(window);
	hovered = true;
}

void UISlider::onMouseLeave(Window& window)
{
	UIElement::onMouseLeave(window);
	hovered = false;
	if(!window.pressed(InputId(InputType::Mouse, 0)))
	{
		dragging = false;
		active = false;
	}
}


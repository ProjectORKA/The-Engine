#pragma once

#include "UIElement.hpp"

struct UISlider final : UIElement
{
	Float minValue      = 0.0f;
	Float maxValue      = 1.0f;
	Float currentValue  = 0.0f;
	Bool  dragging      = false;
	Bool  showValueText = true;

	String            labelText;
	Float*            boundValue = nullptr;
	Function<void(Float)> onValueChanged;

	UISlider();

	UISlider& setLabel(const String& label);
	UISlider& setRange(Float minValue, Float maxValue);
	UISlider& bind(Float& valueRef);
	UISlider& setValue(Float value);
	UISlider& setOnValueChanged(Function<void(Float)> callback);

	[[nodiscard]] Float getValue() const;

	void update(Window& window) override;
	void render(Window& window) override;
	void inputEvent(Window& window, InputEvent& input) override;
	void onMouseEnter(Window& window) override;
	void onMouseLeave(Window& window) override;

private:
	void  syncBoundValue();
	void  setFromMouse(Window& window);
	void  commitValue(Float value);
	Float normalizedValue() const;
};


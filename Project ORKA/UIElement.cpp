#include "UIElement.hpp"
#include "UserInterface.hpp"
#include "Profiler.hpp"

Index nextInteractiveElementId = 0;

void Constraints::update(TiledRectangle& area) const
{
	if(paddingX != static_cast<U16>(-1))
	{
		area.size.x -= 2 * paddingX;
		area.position.x += paddingX;
	}
	if(paddingY != static_cast<U16>(-1))
	{
		area.size.y -= 2 * paddingY;
		area.position.y += paddingY;
	}
	if(width != static_cast<U16>(-1)) area.size.x = width;
	if(height != static_cast<U16>(-1)) area.size.y = height;
}

UiElement::~UiElement() = default;

void UiElement::update(Window& window) {}

void UiElement::destroy(Window& window) {}

void UiElement::create(ResourceManager& resourceManager, Window& window) {}

void UiElement::inputEvent(Window& window, InputEvent input) {}

void UiElement::render(ResourceManager& resourceManager, Window& window, TiledRectangle area) {}

void UiElement::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area)
{
	OPTICK_EVENT();
}

UiElement& UiElement::padding(const U16 width)
{
	constraints.paddingX = width;
	constraints.paddingY = width;
	return *this;
}
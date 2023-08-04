#include "UIElement.hpp"
#include "UserInterface.hpp"

Index nextInteractiveElementId = 0;

UIElement::~UIElement() = default;

UIElement& UIElement::padding(const U16 width)
{
	constraints.paddingX = width;
	constraints.paddingY = width;
	return *this;
}

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

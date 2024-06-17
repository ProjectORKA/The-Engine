#include "UIElement.hpp"
#include "UserInterface.hpp"

Index nextInteractiveElementId = 0;

UIElement::~UIElement() = default;

UIElement& UIElement::fill(const Color color) {
	hasBackground   = true;
	backgroundColor = color;
	return *this;
}

UIElement& UIElement::padding(const Int width)
{
	constraints.paddingX = width;
	constraints.paddingY = width;
	return *this;
}

UIElement& UIElement::name(const String& debugName) {
	this->debugName = debugName;
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

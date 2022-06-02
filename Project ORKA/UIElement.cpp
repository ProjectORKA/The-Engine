
#include "UIElement.hpp"
#include "UserInterface.hpp"

Index nextInteractiveElementID = 0;

UIElement::~UIElement() {
	if (ui.currentlyActive == this) {
		ui.currentlyActive = nullptr;
	}
}

UIElement& UIElement::padding(U16 width) {
	constraints.paddingX = width;
	constraints.paddingY = width;
	return *this;
}


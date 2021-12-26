
#include "UIElement.hpp"
#include "UserInterface.hpp"

Index newUIelementID = 0;
UIElement::~UIElement() {
	if (ui.currentlyActive == this) {
		ui.currentlyActive = nullptr;
	}
}
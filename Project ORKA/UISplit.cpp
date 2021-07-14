
#include "UISplit.hpp"

void renderUISplit(UIElement* element, Window& window, TiledRectangle screenArea)
{
	UISplit* split = static_cast<UISplit*>(element);
	Float percentage = 1.0 / element->content.size();

	if (split->vertical) {
		for (Int i = 0; i < element->content.size(); i++) {
			TiledRectangle area;
			area.position.x = ceil(screenArea.position.x + (screenArea.size.x * percentage * i));
			area.position.y = ceil(screenArea.position.y);
			area.size.x = ceil(screenArea.size.x * percentage);
			area.size.y = ceil(screenArea.size.y);

			element->content[i]->render(window, area);
		}
	}
	else {
		for (Int i = 0; i < element->content.size(); i++) {
			TiledRectangle area;
			area.position.x = ceil(screenArea.position.x);
			area.position.y = ceil(screenArea.position.y + screenArea.size.y * percentage * i);
			area.size.x = ceil(screenArea.size.x);
			area.size.y = ceil(screenArea.size.y * percentage);

			element->content[i]->render(window, area);
		}
	}
}

UISplit::UISplit() {
	destroyFunction = destroyUIElement;
	updateFunction = updateUIElement;
	createFunction = createUIElement;
	renderFunction = renderUISplit;
}
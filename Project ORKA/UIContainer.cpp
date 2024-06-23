#include "UIContainer.hpp"
#include "Window.hpp"

void UIContainer::update(Window& window) {
	for(UIElement* c : contents) c->update(window);
}

void UIContainer::destroy(Window& window) {
	for(UIElement* c : contents) c->destroy(window);
}

void UIContainer::inputEvent(Window& window, const InputEvent input) {
	for(UIElement* c : contents) c->inputEvent(window, input);
}

void UIContainer::create(Window& window) {
	for(UIElement* c : contents) c->create(window);
}

void UIContainer::render(Window& window, const TiledRectangle renderArea)
{
	if(hasBackground)
	{
		DepthTestOverride override(false, window.renderer);
		window.renderer.screenSpace();
		window.renderer.useShader("color");
		window.renderer.fill(backgroundColor);
		window.renderer.uniforms().setMMatrix(matrixFromPositionAndSize(renderArea.position + IVec2(20), renderArea.size - IVec2(40)));
		window.renderer.plane();
	}

	for(UInt i = 0; i < contents.size(); i++)
	{
		TiledRectangle a = renderArea;
		if(renderVertical)
		{
			a.size.y /= contents.size();
			a.position.y -= a.size.y * i;
		}
		else
		{
			a.size.x /= contents.size();
			a.position.x += a.size.x * i;
		}

		contents[i]->render(window, a);
	}
}

void UIContainer::renderInteractive(Window& window, const TiledRectangle renderArea) {
	for(UInt i = 0; i < contents.size(); i++)
	{
		TiledRectangle a = renderArea;
		if(renderVertical)
		{
			a.size.y /= contents.size();
			a.position.y += a.size.y * i;
		}
		else
		{
			a.size.x /= contents.size();
			a.position.x += a.size.x * i;
		}
		contents[i]->renderInteractive(window, a);
	}
}

Bool UIContainer::isEmpty() const {
	return contents.empty();
}

void UIContainer::drop(const UIElement* element) {
	for(Int i = 0; i < contents.size(); i++) if(contents[i] == element) contents.erase(contents.begin() + i);
}

UIContainer& UIContainer::vertical() {
	renderVertical = true;
	return *this;
}

UIContainer& UIContainer::horizontal() {
	renderVertical = false;
	return *this;
}

void UIContainer::add(UIElement& element) {
	contents.push_back(&element);
}

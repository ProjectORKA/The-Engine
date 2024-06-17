#include "UISandbox.hpp"
#include "Window.hpp"

void Toggle::doThis()
{
	toggle = !toggle;
}

void Toggle::destroy(Window& window) {}

void Toggle::update(Window& window)
{
	if(content) content->update(window);
}

//void UISandboxRenderer::destroy(Window& window) {}
//
//void UISandboxRenderer::connect(GameSimulation& simulation) {}
//
//void UISandboxRenderer::update(Window& window)
//{
//	UIContainer::update(window);
//}

void Toggle::create(Window& window) {}

//void UISandboxRenderer::inputEvent(Window& window, const InputEvent input) {}
//
//void UISandboxRenderer::create(Window& window)
//{
//	UIContainer::create(window);
//}
//
//void UISandboxRenderer::render(Window& window, const TiledRectangle area)
//{
//	const Renderer& renderer = window.renderer;
//
//	renderer.clearBackground(Color(0.008, 0.008, 0.009, 1));
//
//	UIContainer::render(window, area);
//}
//
//void UISandboxRenderer::renderInteractive(Window& window, TiledRectangle area) {}

#include "UISandbox.hpp"
#include "Window.hpp"

void Toggle::doThis()
{
	toggle = !toggle;
}

void UISandbox::run()
{
	resourceManager.create();
	ui.create();
	ui.window("ORKA UI Sandbox", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, renderer, resourceManager);
	ui.run();
}

void Toggle::destroy(Window& window) {}

void Toggle::update(Window& window)
{
	if(content) content->update(window);
}

void UISandboxRenderer::destroy(Window& window) {}

void UISandboxRenderer::connect(GameSimulation& simulation) {}

void UISandboxRenderer::update(Window& window)
{
	pauseButton.update(window);
	saveButton.update(window);
}

void Toggle::create(ResourceManager& resourceManager, Window& window) {}

void UISandboxRenderer::inputEvent(Window& window, const InputEvent input)
{
	saveButton.inputEvent(window, input);
	pauseButton.inputEvent(window, input);

	paused = pauseButton.toggle;
}

void UISandboxRenderer::create(ResourceManager& resourceManager, Window& window)
{
	pauseButton.padding(50);
}

void UISandboxRenderer::render(ResourceManager& resourceManager, Window& window, TiledRectangle area)
{
	const Renderer& renderer = window.renderer;

	renderer.clearBackground(Color(0.008, 0.008, 0.009, 1));

	pauseButton.render(resourceManager, window, area);

	if(paused)
	{
		area.size.x /= 2;
		area.size.y /= 2;

		saveButton.render(resourceManager, window, area);
	}
}

void UISandboxRenderer::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area)
{
	pauseButton.renderInteractive(resourceManager, window, area);

	if(paused)
	{
		area.size.x /= 2;
		area.size.y /= 2;

		saveButton.renderInteractive(resourceManager, window, area);
	}
}

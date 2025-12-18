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

void Toggle::create(Window& window) {}

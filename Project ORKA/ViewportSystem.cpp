#pragma once

#include "ViewportSystem.hpp"

Viewport& ViewportSystem::current()
{
	Viewport & viewport = viewports[currentViewport];
	return viewport;
}
void ViewportSystem::create()
{
	add("full",    0, 0,   1, 1);
	add("left",	   0, 0, 0.5, 1);
	add("right", 0.5, 0, 0.5, 1);
}
void ViewportSystem::destroy()
{
	viewports.clear();
	viewportNames.clear();
}
void ViewportSystem::select(String name)
{
	auto it = viewportNames.find(name);
	if (it != viewportNames.end()) currentViewport = it->second;
}
void ViewportSystem::render(Int adaptiveWidth, Int adaptiveHeight)
{
	viewports[currentViewport].update(adaptiveWidth, adaptiveHeight);
	viewports[currentViewport].render();
}
void ViewportSystem::add(String name, Float x, Float y, Float w, Float h)
{
	viewports.emplace_back();
	viewports.back().relativeX = x;
	viewports.back().relativeY = y;
	viewports.back().relativeWidth = w;
	viewports.back().relativeHeight = h;
	viewportNames[name] = viewports.size() - 1;
}

void Viewport::render() {
#ifdef GRAPHICS_API_OPENGL
	glViewport(absoluteX, absoluteY, absoluteWidth, absoluteHeight);
#endif // GRAPHICS_API_OPENGL
}
float Viewport::aspectRatio() {
	return float(absoluteWidth) / float(absoluteHeight);
};
void Viewport::update(Int & adaptiveWidth, Int & adaptiveHeight)
{
	absoluteX		= adaptiveWidth * relativeX;
	absoluteY		= adaptiveHeight * relativeY;
	absoluteWidth	= max(1, adaptiveWidth * relativeWidth);
	absoluteHeight	= max(1, adaptiveHeight * relativeHeight);
}
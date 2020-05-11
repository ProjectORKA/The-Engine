#pragma once

#include "ViewportSystem.hpp"

void ViewportSystem::create()
{
	add("full",    0, 0,   1, 1);
	add("left",	   0, 0, 0.5, 1);
	add("right", 0.5, 0, 0.5, 1);
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

void ViewportSystem::select(String name)
{
	auto it = viewportNames.find(name);
	if (it != viewportNames.end()) currentViewport = it->second;
}

void ViewportSystem::render(Framebuffer& framebuffer)
{
	viewports[currentViewport].update(framebuffer);
	viewports[currentViewport].render();
}

Viewport& ViewportSystem::current()
{
	return viewports[currentViewport];
}

void ViewportSystem::destroy()
{
	viewports.clear();
	viewportNames.clear();
}


void Viewport::update(Framebuffer& framebuffer)
{
	absoluteX		= framebuffer.size.x * relativeX;
	absoluteY		= framebuffer.size.y * relativeY;
	absoluteWidth	= framebuffer.size.x * relativeWidth;
	absoluteHeight	= framebuffer.size.y * relativeHeight;
}

void Viewport::render() {
#ifdef GRAPHICS_API_OPENGL
	glViewport(absoluteX, absoluteY, absoluteWidth, absoluteHeight);
#endif // GRAPHICS_API_OPENGL
}

float Viewport::aspectRatio() {
	return float(absoluteWidth) / float(absoluteHeight);
};
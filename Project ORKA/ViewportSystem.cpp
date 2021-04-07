#pragma once

#include "ViewportSystem.hpp"
#include "Math.hpp"

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
void ViewportSystem::render(Framebuffer & framebuffer)
{
	viewports[currentViewport].update(framebuffer.width, framebuffer.height);
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
	glViewport(absoluteX, absoluteY, absoluteWidth, absoluteHeight);
}
float Viewport::aspectRatio() {
	return float(absoluteWidth) / float(absoluteHeight);
};
void Viewport::update(Int & framebufferWidth, Int & framebufferHeight)
{
	absoluteX		= framebufferWidth * relativeX;
	absoluteY		= framebufferHeight * relativeY;
	absoluteWidth	= max<int>(1, framebufferWidth * relativeWidth);
	absoluteHeight	= max<int>(1, framebufferHeight * relativeHeight);
}
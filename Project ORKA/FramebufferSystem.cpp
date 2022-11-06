
#include "FramebufferSystem.hpp"
#include "Renderer.hpp"

void FramebufferSystem::destroy()
{
	deselect();
	for (auto f : framebuffers) f.destroy();
}
void FramebufferSystem::deselect()
{
	apiBindDrawFramebuffer(0);
}
void FramebufferSystem::drawToWindow() {
	apiBindDrawFramebuffer(0);
}
void FramebufferSystem::update(Area area)
{
	area.setMinimum(1);
	framebufferSize = area;
	for (Framebuffer& f : framebuffers) {
		if(f.resizeable) f.resize(area);
	}
}
void FramebufferSystem::addGbuffer(Name name) {
	framebuffers.emplace_back();
	framebuffers.back().create(Area(1));
	framebuffers.back().resizeable = true;
	framebuffers.back().add(4, dataTypeFloat, 0);	//color
	framebuffers.back().add(3, dataTypeFloat, 1);	//
	framebuffers.back().add(3, dataTypeFloat, 2);	//
	framebuffers.back().add(1, dataTypeUInt, 3);	//
	framebuffers.back().add(5, dataTypeFloat, 4);	//depth
	nametoID[name] = framebuffers.size() - 1;
}
Framebuffer& FramebufferSystem::currentRead()
{
	return framebuffers[currentReadFramebufferIndex];
}
Framebuffer& FramebufferSystem::currentDraw()
{
	return framebuffers[currentDrawFramebufferIndex];
}
void FramebufferSystem::addIDBuffer(Name name) {
	framebuffers.emplace_back();
	framebuffers.back().resizeable = true;
	framebuffers.back().create(Area(1));
	framebuffers.back().add(3, dataTypeUInt, 0);	//ids
	framebuffers.back().add(5, dataTypeFloat, 1);	//depth
	nametoID[name] = framebuffers.size() - 1;
}
void FramebufferSystem::addFrameBuffer(Name name) {
	framebuffers.emplace_back();
	framebuffers.back().create(Area(1));
	framebuffers.back().resizeable = true;
	framebuffers.back().add(4, dataTypeFloat, 0);	//color
	framebuffers.back().add(5, dataTypeFloat, 1);	//depth
	nametoID[name] = framebuffers.size() - 1;
}
void FramebufferSystem::draw(Renderer& renderer, Name name) {
	draw(renderer, nametoID[name]);
}
void FramebufferSystem::read(Renderer& renderer, Name name) {
	read(renderer, nametoID[name]);
}
void FramebufferSystem::create(Renderer& renderer, Area size) {
	//gbuffer
	addGbuffer("main");
	addGbuffer("postProcess");
}
void  FramebufferSystem::read(Renderer& renderer, Index framebufferIndex) {
	if (framebufferIndex > (framebuffers.size() - 1)) {
		logError(String("Invalid framebufferIndex! (").append(toString(framebufferIndex)).append(")"));
		return;
	}
	currentReadFramebufferIndex = framebufferIndex;
	currentRead().read();
}
void  FramebufferSystem::draw(Renderer& renderer, Index framebufferIndex) {
	if (framebufferIndex > (framebuffers.size() - 1)) {
		logError(String("Invalid framebufferIndex! (").append(toString(framebufferIndex)).append(")"));
		return;
	}
	currentDrawFramebufferIndex = framebufferIndex;
	currentDraw().draw();
}
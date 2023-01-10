
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
	apiViewport(0, 0, windowSize.x, windowSize.y);
	apiScissor(0, 0, windowSize.x, windowSize.y);
}
void FramebufferSystem::update(Area area)
{
	area.setMinimum(1);
	windowSize = area;
	for (Framebuffer& f : framebuffers) {
		if (f.dynamicResize) f.resize(area * f.relativeScale);
	}
}
Framebuffer& FramebufferSystem::addGbuffer(Name name, Float relativeSize) {
	Framebuffer & f = framebuffers.emplaceBack();
	f.create(Area(1));
	f.add(4, dataTypeFloat, 0);	//color
	f.add(3, dataTypeFloat, 1);	//
	f.add(3, dataTypeFloat, 2);	//
	f.add(1, dataTypeUInt, 3);	//
	f.add(5, dataTypeFloat, 4);	//depth
	f.relativeScale = relativeSize;
	nametoID[name] = framebuffers.size() - 1;
	return f;
}
Framebuffer& FramebufferSystem::currentRead()
{
	return framebuffers[currentReadFramebufferIndex];
}
Framebuffer& FramebufferSystem::currentDraw()
{
	return framebuffers[currentDrawFramebufferIndex];
}
Framebuffer& FramebufferSystem::addIDBuffer(Name name, Float relativeSize) {
	Framebuffer& f = framebuffers.emplaceBack();
	f.create(Area(1));
	f.add(3, dataTypeUInt, 0);	//ids
	f.add(5, dataTypeFloat, 1);	//depth
	f.relativeScale = relativeSize;
	nametoID[name] = framebuffers.size() - 1;
	return f;
}
Framebuffer& FramebufferSystem::addFrameBuffer(Name name, Float relativeSize) {
	Framebuffer& f = framebuffers.emplaceBack();
	f.create(Area(1));
	f.add(4, dataTypeFloat, 0);	//color
	f.add(5, dataTypeFloat, 1);	//depth
	f.relativeScale = relativeSize;
	nametoID[name] = framebuffers.size() - 1;
	return f;
}
void FramebufferSystem::draw(Renderer& renderer, Name name) {
	draw(renderer, nametoID[name]);
}
void FramebufferSystem::read(Renderer& renderer, Name name) {
	read(renderer, nametoID[name]);
}
void FramebufferSystem::create(Renderer& renderer, Area size) {
	//gbuffer
	addGbuffer("main", 1);
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
	renderer.uniforms().framebufferWidth(currentDraw().size.x);
	renderer.uniforms().framebufferHeight(currentDraw().size.y);
}
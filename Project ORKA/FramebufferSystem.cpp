
#include "FramebufferSystem.hpp"
#include "Renderer.hpp"

//use when making a framebuffer system
void FramebufferSystem::destroy()
{
	drawToWindow();
	for (auto f : framebuffers) f.destroy();
}

//use when destroying it
void FramebufferSystem::create(Renderer& renderer, Area size) {
	//gbuffer
	addGbuffer("main");
	addGbuffer("postProcess");
}

//use when trying to draw to window (sets selected framebuffer to 0)
void FramebufferSystem::drawToWindow() {
	apiBindDrawFramebuffer(0);
}

//use before rendering anything to resize framebuffers if needed
void FramebufferSystem::update(Area area)
{
	area.setMinimum(1);
	framebufferSize = area;
	for (Framebuffer& f : framebuffers) {
		if (f.resizeable) f.resize(area);
	}
}

//adds a Framebuffer with separate buffers for color, normals, roughness, etc
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

//draw and read functions
Framebuffer& FramebufferSystem::currentRead()
{
	return framebuffers[currentReadFramebufferIndex];
}
Framebuffer& FramebufferSystem::currentDraw()
{
	return framebuffers[currentDrawFramebufferIndex];
}
void FramebufferSystem::draw(Name name) {
	draw(nametoID[name]);
}
void FramebufferSystem::read(Name name) {
	read(nametoID[name]);
}
void  FramebufferSystem::read(Index framebufferIndex) {
	if (!isIndexInArray(framebufferIndex, framebuffers)) {
		logError(String("Invalid framebufferIndex! (").append(toString(framebufferIndex)).append(")"));
		return;
	}
	currentReadFramebufferIndex = framebufferIndex;
	currentRead().read();
}
void  FramebufferSystem::draw(Index framebufferIndex) {

	if (!isIndexInArray(framebufferIndex, framebuffers)) {
		logError(String("Invalid framebufferIndex! (").append(toString(framebufferIndex)).append(")"));
		return;
	}
	currentDrawFramebufferIndex = framebufferIndex;
	currentDraw().draw();

}
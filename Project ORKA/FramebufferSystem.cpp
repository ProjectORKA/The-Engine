#include "FramebufferSystem.hpp"
#include "Renderer.hpp"

void FramebufferSystem::destroy() {
	deselect();
	for (auto f : framebuffers) f.destroy();
}

void FramebufferSystem::deselect() { apiBindDrawFramebuffer(0); }

void FramebufferSystem::drawToWindow() const {
	apiBindDrawFramebuffer(0);
	apiViewport(0, 0, windowSize.x, windowSize.y);
	apiScissor(0, 0, windowSize.x, windowSize.y);
}

void FramebufferSystem::update(Area area) {
	area.setMinimum(1);
	windowSize = area;
	for (Framebuffer& f : framebuffers) { if (f.dynamicResize) f.resize(area * f.relativeScale); }
}

Framebuffer& FramebufferSystem::addGbuffer(const Name& name, const Float relativeSize) {
	Framebuffer& f = framebuffers.emplace_back();
	f.create(Area(1));
	f.add(4, DataType::dataTypeFloat, 0); //color
	f.add(3, DataType::dataTypeFloat, 1); //normal
	f.add(3, DataType::dataTypeFloat, 2); //world position
	f.add(1, DataType::dataTypeUInt, 3); //
	f.add(5, DataType::dataTypeFloat, 4); //depth
	f.relativeScale = relativeSize;
	nametoID[name] = framebuffers.size() - 1;
	return f;
}

Framebuffer& FramebufferSystem::currentRead() { return framebuffers[currentReadFramebufferIndex]; }
Framebuffer& FramebufferSystem::currentDraw() { return framebuffers[currentDrawFramebufferIndex]; }

Framebuffer& FramebufferSystem::addIDBuffer(const Name& name, const Float relativeSize) {
	Framebuffer& f = framebuffers.emplace_back();
	f.create(Area(1));
	f.add(3, DataType::dataTypeUInt, 0); //ids
	f.add(5, DataType::dataTypeFloat, 1); //depth
	f.relativeScale = relativeSize;
	nametoID[name] = framebuffers.size() - 1;
	return f;
}

Framebuffer& FramebufferSystem::addFrameBuffer(const Name& name, const Float relativeSize) {
	Framebuffer& f = framebuffers.emplace_back();
	f.create(Area(1));
	f.add(4, DataType::dataTypeFloat, 0); //color
	f.add(5, DataType::dataTypeFloat, 1); //depth
	f.relativeScale = relativeSize;
	nametoID[name] = framebuffers.size() - 1;
	return f;
}

void FramebufferSystem::draw(Renderer& renderer, const Name& name) { draw(renderer, nametoID[name]); }
void FramebufferSystem::read(Renderer& renderer, const Name& name) { read(renderer, nametoID[name]); }

void FramebufferSystem::create(Renderer& renderer, Area size) {
	//gbuffer
	addGbuffer("main");
	addGbuffer("postProcess");
}

void FramebufferSystem::read(Renderer& renderer, const Index framebufferIndex) {
	if (framebufferIndex > (framebuffers.size() - 1)) {
		logError(String("Invalid framebufferIndex! (").append(toString(framebufferIndex)).append(")"));
		return;
	}
	currentReadFramebufferIndex = framebufferIndex;
	currentRead().read();
}

void FramebufferSystem::draw(Renderer& renderer, const Index framebufferIndex) {
	if (framebufferIndex > (framebuffers.size() - 1)) {
		logError(String("Invalid framebufferIndex! (").append(toString(framebufferIndex)).append(")"));
		return;
	}
	currentDrawFramebufferIndex = framebufferIndex;
	currentDraw().draw();
	renderer.uniforms().framebufferWidth(currentDraw().size.x);
	renderer.uniforms().framebufferHeight(currentDraw().size.y);
}

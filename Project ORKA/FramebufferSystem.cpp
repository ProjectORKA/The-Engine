
#include "FramebufferSystem.hpp"
#include "Renderer.hpp"

void FramebufferSystem::destroy()
{
	deselect();
	framebuffers.clear();
}
void FramebufferSystem::deselect()
{
	apiBindFramebuffer(0);
}
Framebuffer& FramebufferSystem::current()
{
	return *framebuffers[currentFramebufferIndex];
}
void FramebufferSystem::update(Area area)
{
	area.clamp(1);

	if (framebufferSize != area) {
		framebufferSize = area;
		for (auto f : framebuffers) {
			f->resize(framebufferSize);
		}
	}
}
IDFrameBuffer& FramebufferSystem::idFramebuffer() {
	return *idFbPtr;
}
void FramebufferSystem::add(Framebuffer * framebuffer)
{
	framebuffers.push_back(framebuffer);
}
void FramebufferSystem::create(Renderer& renderer, Area size) {
	framebuffers.push_back(new GBuffer());

	IDFrameBuffer* pointer = new IDFrameBuffer();
	idFbPtr = pointer;
	framebuffers.push_back(pointer);
	idFramebuffer().add(4, dataTypeUInt, 0);
	idFramebuffer().add(5, dataTypeFloat, 1);
}
void FramebufferSystem::use(Renderer & renderer, Index framebufferIndex)
{
	if (framebufferIndex > (framebuffers.size() - 1)) {
		logError(String("Invalid framebufferIndex! (").append(toString(framebufferIndex)).append(")"));
		return;
	}
	currentFramebufferIndex = framebufferIndex;
	current().use();
}
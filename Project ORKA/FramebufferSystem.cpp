
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
void FramebufferSystem::add(Framebuffer * framebuffer)
{
	framebuffers.push_back(framebuffer);
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
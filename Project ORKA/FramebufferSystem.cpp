
#include "FramebufferSystem.hpp"
#include "Renderer.hpp"

void FramebufferSystem::destroy()
{
	deselect();
	for (Framebuffer& framebuffer : framebuffers) {
		framebuffer.destroy();
	}
	framebuffers.clear();
	idFramebuffer.destroy();
}
void FramebufferSystem::deselect()
{
	apiBindFramebuffer(0);
}
Framebuffer& FramebufferSystem::current()
{
	return framebuffers[currentFramebufferIndex];
}
void FramebufferSystem::update(Area area)
{
	area.clamp(1);

	if (framebufferSize != area) {
		framebufferSize = area;
		for (Framebuffer& framebuffer : framebuffers) {
			framebuffer.resize(framebufferSize);
		}
		idFramebuffer.resize(framebufferSize);
	}
}
void FramebufferSystem::add(Renderer & renderer)
{
	framebuffers.emplace_back();
	framebuffers.back().create(framebufferSize);
	use(renderer, framebuffers.size() - 1);
}
void FramebufferSystem::create(Renderer& renderer, Area size)
{
	this->framebufferSize = size;
	framebuffers.clear();
	idFramebuffer.create(framebufferSize);
	add(renderer);	//main framebuffer
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
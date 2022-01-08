#include "FramebufferSystem.hpp"
#include "Renderer.hpp"

Framebuffer& FramebufferSystem::current()
{
	return framebuffers[currentFramebufferIndex];
}

void FramebufferSystem::add(Renderer & renderer)
{
	framebuffers.emplace_back();
	framebuffers.back().create();
	use(renderer, framebuffers.size() - 1);
}
void FramebufferSystem::create(Renderer& renderer)
{
	framebuffers.clear();
	add(renderer);	//0 main framebuffer
	add(renderer);	//1 blur result
	add(renderer);	//2 mask
	add(renderer);	//3 drop shadow
}
void FramebufferSystem::destroy()
{
	deselect();
	for (Framebuffer& framebuffer : framebuffers) {
		framebuffer.destroy();
	}
	framebuffers.clear();
}
void FramebufferSystem::deselect()
{
	apiBindFramebuffer(0);
}
void FramebufferSystem::update(Area area)
{
	area.clamp(1);

	if (framebufferSize != area) {
		framebufferSize = area;
		for (Framebuffer& framebuffer : framebuffers) {
			framebuffer.resize(framebufferSize);
		}
	}
}
void FramebufferSystem::use(Renderer & renderer, Index framebufferIndex)
{
	if (framebufferIndex > (framebuffers.size() - 1)) {
		logError(String("Invalid framebufferIndex! (").append(std::to_string(framebufferIndex)).append(")"));
		return;
	}
	currentFramebufferIndex = framebufferIndex;
	current().use(renderer);
}
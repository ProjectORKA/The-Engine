#include "FramebufferSystem.hpp"

Framebuffer& FramebufferSystem::current()
{
	return framebuffers[currentFramebufferIndex];
}

void FramebufferSystem::add()
{
	framebuffers.emplace_back();
	framebuffers.back().create();
	use(framebuffers.size() - 1);
}
void FramebufferSystem::create()
{
	framebuffers.clear();
	add();	//0 main framebuffer
	add();	//1 blur result
	add();	//2 mask
	add();	//3 drop shadow
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
	apiBindFramebuffer(GL_FRAMEBUFFER, 0);
	//apiDrawBuffer(GL_BACK); //[TODO] check if unnecessary
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
void FramebufferSystem::use(Index framebufferIndex)
{
	if (framebufferIndex > (framebuffers.size() - 1)) {
		logError(String("Invalid framebufferIndex! (").append(std::to_string(framebufferIndex)).append(")"));
		return;
	}
	currentFramebufferIndex = framebufferIndex;
	current().use();
}
#include "FramebufferSystem.hpp"

Framebuffer& FramebufferSystem::current()
{
	return framebuffers[currentFramebufferIndex];
}

void FramebufferSystem::add()
{
	framebuffers.emplace_back();
	framebuffers.back().create();
	select(framebuffers.size() - 1);
}
void FramebufferSystem::create()
{
	framebuffers.clear();
	add();	//0 main framebuffer
	add();	//1 blur result
	add();	//2 mask
	add();	//3 drop shadow
}
void FramebufferSystem::update()
{
	if (needsUpdate) {
		for (Framebuffer& framebuffer : framebuffers) {
			framebuffer.resize(currentFramebufferSize);
		}
		needsUpdate = false;
	}
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
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);
}
void FramebufferSystem::updateSizes(Area area)
{
	area.x = max(area.x, 1);
	area.y = max(area.y, 1);

	if (currentFramebufferSize != area) {
		currentFramebufferSize = area;
		needsUpdate = true;
	}
}
void FramebufferSystem::select(Index framebufferIndex)
{
	if (framebufferIndex > (framebuffers.size() - 1)) {
		logError(String("Invalid framebufferIndex! (").append(std::to_string(framebufferIndex)).append(")"));
		return;
	}
	currentFramebufferIndex = framebufferIndex;
	current().use();
}
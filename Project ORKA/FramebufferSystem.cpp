#include "FramebufferSystem.hpp"

void FramebufferSystem::create()
{
	framebuffers.clear();
	add(0);
	add(2);
	add(4);
	add(8);
	add(16);
}
void FramebufferSystem::update()
{
	if (needsUpdate) {
		for (Framebuffer& framebuffer : framebuffers) {
			framebuffer.resize(Vec2(windowWidth, windowHeight));
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
void FramebufferSystem::add(UShort samples)
{
	framebuffers.emplace_back();
	framebuffers.back().samples = samples;
	framebuffers.back().create();
	select(framebuffers.size() - 1);
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
void FramebufferSystem::updateSizes(UInt width, UInt height)
{
	if (windowWidth != width) {
		windowWidth = width;
		needsUpdate = true;
	}
	if (windowHeight != height) {
		windowHeight = height;
		needsUpdate = true;
	}
}

Framebuffer& FramebufferSystem::current()
{
	return framebuffers[currentFramebufferIndex];
}
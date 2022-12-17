
#include "IdFramebuffer.hpp"
#include "FramebufferSystem.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include "Profiler.hpp"

void IdFramebuffer::read(Renderer& renderer)
{
	renderer.framebufferSystem.read(framebufferID);
}
void IdFramebuffer::draw(Renderer& renderer)
{
	OPTICK_EVENT();
	renderer.framebufferSystem.draw(framebufferID);
}
void IdFramebuffer::create(Renderer& renderer)
{
	renderer.framebufferSystem.addIDBuffer("idBuffer");

	//idframebuffer
	framebufferID = renderer.framebufferSystem.framebuffers.size() - 1;
	renderer.framebufferSystem.nametoID["id"] = framebufferID;
}
void IdFramebuffer::updateIdsUnderCursor(Window& window)
{
	read(window.renderer);
	Vector<UInt> idData;

	if (window.capturing) {
		idData = window.renderer.framebufferSystem.currentRead().readPixelsAtCenterUIntRGB(0);
	}
	else {
		Vec2 curPos = window.mousePosBotLeft;
		idData = window.renderer.framebufferSystem.currentRead().readPixelsUIntRGB(curPos.x, curPos.y, 0);
	}

	objectID = idData[0];
	instanceID = idData[1];
	triangleID = idData[2];
}
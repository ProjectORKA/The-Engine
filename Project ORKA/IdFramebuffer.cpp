
#include "IdFramebuffer.hpp"
#include "FramebufferSystem.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include "Profiler.hpp"

void IdFramebuffer::read(Renderer& renderer) const
{
	renderer.framebufferSystem.read(renderer, framebufferId);
}

void IdFramebuffer::draw(Renderer& renderer) const
{
	OPTICK_EVENT();
	renderer.framebufferSystem.draw(renderer, framebufferId);
}

void IdFramebuffer::create(Renderer& renderer)
{
	renderer.framebufferSystem.addIdBuffer("idBuffer");

	//idframebuffer
	framebufferId = renderer.framebufferSystem.framebuffers.size() - 1;
	renderer.framebufferSystem.nametoId["id"] = framebufferId;
}

void IdFramebuffer::updateIdsUnderCursor(Window& window)
{
	read(window.renderer);
	Vector<UInt> idData;

	if(window.capturing)
	{
		idData = window.renderer.framebufferSystem.currentRead().readPixelsAtCenterUIntRgb(0);
	}
	else
	{
		const Vec2 curPos = window.mousePosBotLeft;
		idData = window.renderer.framebufferSystem.currentRead().readPixelsUIntRgb(curPos.x, curPos.y, 0);
	}

	objectId = idData[0];
	instanceId = idData[1];
	triangleId = idData[2];
}
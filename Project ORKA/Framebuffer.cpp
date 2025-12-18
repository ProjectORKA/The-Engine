#include "Framebuffer.hpp"
#include "Renderer.hpp"

void Framebuffer::destroy(Renderer& renderer)
{
	// destroys the framebuffer and resets it to the default values just in case
	if (debugFramebuffersIsEnabled)
	{
		logDebug("Destroying Framebuffer: " + name);
		if (!complete) logError("Framebuffer not complete!");
	}

	openglFramebuffer.destroy(renderer.openGlContext);
	for (auto f : framebufferTextures) f.destroy(renderer);
	framebufferTextures.clear();
	name = "EmptyFramebuffer";
	drawBuffers.clear();
	size     = IVec2(1);
	complete = false;
}
void Framebuffer::clear(Renderer& renderer) const
{
	if (debugFramebuffersIsEnabled) if (!complete) logError("Framebuffer not complete!");
	for (const FramebufferTexture& framebufferTexture : framebufferTextures)
	{
		if (framebufferTexture.isColor()) openglFramebuffer.clearColor(renderer.openGlContext, framebufferTexture.getAttachmentSlot(), Color(0, 0, 0, 1));
		else openglFramebuffer.clearDepth(renderer.openGlContext, 1.0f);
	}
}
IVec2 Framebuffer::getSize() const
{
	return {size};
}
Bool Framebuffer::isComplete() const
{
	return complete;
}
Int Framebuffer::getWidth() const
{
	return size.x;
}
void Framebuffer::checkComplete(Renderer& renderer)
{
	// makes sure the framebuffer is complete
	if (openglFramebuffer.isComplete(renderer.openGlContext)) complete = true;
	else
	{
		complete = false;
		logError("Framebuffer not complete!");
	}
}
Int Framebuffer::getHeight() const
{
	return size.y;
}
//[TODO] turn this into r.bindDraw(framebuffer)
void Framebuffer::bindDraw(Renderer& renderer) const
{
	if (debugFramebuffersIsEnabled) if (!complete) logError("Framebuffer not complete!");

	openglFramebuffer.bindDraw(renderer.openGlContext);
	OpenGL::apiViewport(renderer.openGlContext, 0, 0, size.x, size.y);
	OpenGL::apiScissor(renderer.openGlContext, 0, 0, size.x, size.y);
}
// [TODO] remove this from code that doesn't read pixels (aka. postprocessing)
void Framebuffer::bindRead(Renderer& renderer) const
{
	if (debugFramebuffersIsEnabled) if (!complete) logError("Framebuffer not complete!");

	openglFramebuffer.bindRead(renderer.openGlContext);
}
void Framebuffer::clearDepth(Renderer& renderer) const
{
	if (debugFramebuffersIsEnabled) if (!complete) logError("Framebuffer not complete!");

	constexpr Float clearDepth = 1.0f;
	openglFramebuffer.clearDepth(renderer.openGlContext, clearDepth);
}
Float Framebuffer::aspectRatio() const
{
	return static_cast<Float>(size.x) / static_cast<Float>(size.y);
}
void Framebuffer::clearColor(Renderer& renderer, const Vec4 clearColor) const
{
	if (debugFramebuffersIsEnabled) if (!complete) logError("Framebuffer not complete!");

	for (const FramebufferTexture& framebufferTexture : framebufferTextures) if (framebufferTexture.isColor()) openglFramebuffer.clearColor(renderer.openGlContext, framebufferTexture.getAttachmentSlot(), clearColor);
}
void Framebuffer::clearColor(Renderer& renderer, const IVec4 clearColor) const
{
	if (debugFramebuffersIsEnabled) if (!complete) logError("Framebuffer not complete!");

	for (const auto& framebufferTexture : framebufferTextures) if (framebufferTexture.isColor()) openglFramebuffer.clearColor(renderer.openGlContext, framebufferTexture.getAttachmentSlot(), clearColor);
}
void Framebuffer::setAsTexture(Renderer& renderer, const TextureSlot slot) const
{
	// uses the first color attachment of the framebuffer as a texture in the specified slot

	if (debugFramebuffersIsEnabled) if (!complete) logError("Framebuffer not complete!");

	for (const FramebufferTexture& framebufferTexture : framebufferTextures)
	{
		if (framebufferTexture.isColor())
		{
			framebufferTexture.useTextureInSlot(renderer, slot);
			return;
		}
	}
	logError("Framebuffer does not have renderable texture!");
}
void Framebuffer::create(Renderer& renderer, const String& name, const IVec2 dimensions)
{
	if (debugFramebuffersIsEnabled) logDebug("Creating Framebuffer: " + name);

	this->complete = false;
	this->name     = name;
	this->size     = dimensions;
	openglFramebuffer.create(renderer.openGlContext, name);
}
void Framebuffer::resize(Renderer& renderer, const IVec2 resolution)
{
	// resizes a framebuffer
	// all textures have to be deleted and recreated at the appropriate size

	if (debugFramebuffersIsEnabled) if (!complete) logError("Framebuffer (" + name + ") not complete!");

	if (complete)
	{
		// only resize if necessary
		if (size != resolution)
		{
			size = resolution;

			// framebuffer textures will be invalidated and have to be reattached
			drawBuffers.clear(); // TODO: check if this is needed
			for (auto t : framebufferTextures)
			{
				t.resize(renderer, resolution);
				attachTexture(renderer, t);
			}
		}
	}
}
void Framebuffer::attachTexture(Renderer& renderer, const FramebufferTexture& framebufferTexture)
{
	openglFramebuffer.attachTexture(renderer.openGlContext, framebufferTexture.getAttachmentSlot(), framebufferTexture.getGLID());

	if (framebufferTexture.isColor())
	{
		drawBuffers.push_back(static_cast<UInt>(framebufferTexture.getAttachmentSlot()));
		openglFramebuffer.drawBuffers(renderer.openGlContext, drawBuffers);
	}
}
IVec4 Framebuffer::readPixelsAtCenterUIntRGB(Renderer& renderer, const FramebufferMode attachment) const
{
	if (debugFramebuffersIsEnabled) if (!complete) logError("Framebuffer not complete!");

	return readPixelsUIntRGB(renderer, size.x / 2, size.y / 2, attachment);
}
IVec4 Framebuffer::readPixelsUIntRGB(Renderer& renderer, const Int x, const Int y, const FramebufferMode attachment) const
{
	if (debugFramebuffersIsEnabled) if (!complete) logError("Framebuffer not complete!");

	return openglFramebuffer.readPixelIVec4(renderer.openGlContext, x, y, attachment);
}
void Framebuffer::add(Renderer& renderer, const WritePixelsFormat format, const DataType type, const FramebufferAttachment slot, const Bool isDrawnTo, const Wrapping wrapping)
{
	FramebufferTexture framebufferTexture;
	framebufferTexture.create(renderer, size, format, type, slot, wrapping);
	attachTexture(renderer, framebufferTexture);
	framebufferTextures.push_back(framebufferTexture);
	if (!openglFramebuffer.isComplete(renderer.openGlContext)) logError("Framebuffer is not complete!");
}

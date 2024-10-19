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

	openglFramebuffer.destroy();
	for (auto f : framebufferTextures) f.destroy(renderer);
	framebufferTextures.clear();
	name = "EmptyFramebuffer";
	drawBuffers.clear();
	size     = Area(1);
	complete = false;
}
void Framebuffer::clear() const
{
	// clears the framebuffer to be black
	// it loops through all color and depth textures

	if (debugFramebuffersIsEnabled) if (!complete) logError("Framebuffer not complete!");

	for (const FramebufferTexture& framebufferTexture : framebufferTextures)
	{
		if (framebufferTexture.isColor())
		{
			openglFramebuffer.clearColor(framebufferTexture.getAttachmentSlot(), Color(0, 0, 0, 1));
		}
		else
		{
			openglFramebuffer.clearDepth(1.0f);
		}
	}
}
IVec2 Framebuffer::getSize() const
{
	return IVec2(size);
}
Int Framebuffer::getWidth() const
{
	return size.x;
}
void Framebuffer::checkComplete()
{
	// makes sure the framebuffer is complete
	if (openglFramebuffer.isComplete())
	{
		complete = true;
	}
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

	openglFramebuffer.bindDraw();
	OpenGL::apiViewport(renderer.openGlContext, 0, 0, size.x, size.y);
	OpenGL::apiScissor(renderer.openGlContext, 0, 0, size.x, size.y);
}
// [TODO] remove this from code that doesn't read pixels (aka. postprocessing)
void Framebuffer::bindRead() const
{
	if (debugFramebuffersIsEnabled) if (!complete) logError("Framebuffer not complete!");

	openglFramebuffer.bindRead();
}
void Framebuffer::clearDepth() const
{
	if (debugFramebuffersIsEnabled) if (!complete) logError("Framebuffer not complete!");

	constexpr Float clearDepth = 1.0f;
	openglFramebuffer.clearDepth(clearDepth);
}
Float Framebuffer::aspectRatio() const
{
	return static_cast<Float>(size.x) / static_cast<Float>(size.y);
}
void Framebuffer::clearColor(const Vec4 clearColor) const
{
	if (debugFramebuffersIsEnabled) if (!complete) logError("Framebuffer not complete!");

	for (const FramebufferTexture& framebufferTexture : framebufferTextures) if (framebufferTexture.isColor()) openglFramebuffer.clearColor(framebufferTexture.getAttachmentSlot(), clearColor);
}
void Framebuffer::clearColor(const IVec4 clearColor) const
{
	if (debugFramebuffersIsEnabled) if (!complete) logError("Framebuffer not complete!");

	for (const auto& framebufferTexture : framebufferTextures) if (framebufferTexture.isColor()) openglFramebuffer.clearColor(framebufferTexture.getAttachmentSlot(), clearColor);
}
void Framebuffer::setAsTexture(const TextureSlot slot) const
{
	// uses the first color attachment of the framebuffer as a texture in the specified slot

	if (debugFramebuffersIsEnabled) if (!complete) logError("Framebuffer not complete!");

	for (const FramebufferTexture& framebufferTexture : framebufferTextures)
	{
		if (framebufferTexture.isColor())
		{
			framebufferTexture.useTextureInSlot(slot);
			return;
		}
	}
	logError("Framebuffer does not have renderable texture!");
}
void Framebuffer::create(const String& name, const Area dimensions)
{
	if (debugFramebuffersIsEnabled) logDebug("Creating Framebuffer: " + name);

	this->complete = false;
	this->name     = name;
	this->size     = dimensions;
	openglFramebuffer.create(name);
}
void Framebuffer::resize(Renderer& renderer, const Area resolution)
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
				attachTexture(t);
			}
		}
	}
}
void Framebuffer::attachTexture(const FramebufferTexture& framebufferTexture)
{
	openglFramebuffer.attachTexture(framebufferTexture.getAttachmentSlot(), framebufferTexture.getGLID());

	if (framebufferTexture.isColor())
	{
		drawBuffers.push_back(static_cast<UInt>(framebufferTexture.getAttachmentSlot()));
		openglFramebuffer.drawBuffers(drawBuffers);
	}
}
IVec4 Framebuffer::readPixelsAtCenterUIntRGB(const FramebufferMode attachment) const
{
	if (debugFramebuffersIsEnabled) if (!complete) logError("Framebuffer not complete!");

	return readPixelsUIntRGB(size.x / 2, size.y / 2, attachment);
}
IVec4 Framebuffer::readPixelsUIntRGB(const Int x, const Int y, const FramebufferMode attachment) const
{
	if (debugFramebuffersIsEnabled) if (!complete) logError("Framebuffer not complete!");

	return openglFramebuffer.readPixelIVec4(x, y, attachment);
}
void Framebuffer::add(Renderer& renderer, const WritePixelsFormat format, const DataType type, const FramebufferAttachment slot, const Bool isDrawnTo, const Wrapping wrapping)
{
	FramebufferTexture framebufferTexture;
	framebufferTexture.create(renderer, size, format, type, slot, wrapping);
	attachTexture(framebufferTexture);
	framebufferTextures.push_back(framebufferTexture);
	if (!openglFramebuffer.isComplete()) logError("Framebuffer is not complete!");
}

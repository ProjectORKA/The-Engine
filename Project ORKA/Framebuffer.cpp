#include "Framebuffer.hpp"
#include "Renderer.hpp"

//framebuffer
Float Framebuffer::aspectRatio() const
{
	return static_cast<Float>(size.x) / static_cast<Float>(size.y);
}

void Framebuffer::read() const
{
	apiBindReadFramebuffer(framebufferId);
};

void Framebuffer::draw() const
{
	apiBindDrawFramebuffer(framebufferId);
	apiViewport(0, 0, size.x, size.y);
	apiScissor(0, 0, size.x, size.y);
}

void Framebuffer::destroy()
{
	apiDeleteFramebuffer(framebufferId); //doesn't work. ask Nvidia
	framebufferTextures.clear();
}

void Framebuffer::create(const String& name, const Area dimensions)
{
	this->name = name;
	this->size = dimensions;
	apiGenFramebuffer(framebufferId);
	apiBindFramebuffer(framebufferId);
	apiObjectLabel(ObjectLabelType::Framebuffer, framebufferId, name);
}

void Framebuffer::blitFramebuffer() const
{
	apiBindDrawFramebuffer(0);
	apiBindReadFramebuffer(framebufferId);
	apiBlitFramebuffer(size.x, size.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void Framebuffer::resize(const Area resolution)
{
	//apply size
	if(size != resolution)
	{
		size = resolution;
		for(auto t : framebufferTextures) t.resize(resolution);
	}
}

void Framebuffer::setAsTexture(const Index slot) const
{
	for(const auto t : framebufferTextures)
	{
		if(t.isColor)
		{
			t.texture.use(slot);
			return;
		}
	}
	logError("Framebuffer does not have renderable texture!");
}

void Framebuffer::add(const UInt components, const DataType type, const UInt slot)
{
	//create texture and add it to the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
	//apiBindDrawFramebuffer(framebufferId);
	FramebufferTexture t;
	t.texture.load(size, components, type);
	t.texture.attachTexture(slot);
	t.slot = slot;
	framebufferTextures.push_back(t);

	//set up framebuffer for drawing
	UInt a = 0;
	Vector<UInt> drawBuffers;
	for(const FramebufferTexture& framebufferTexture : framebufferTextures)
	{
		if(framebufferTexture.isColor) drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + a++);
	}

	apiDrawBuffers(drawBuffers);
	glReadBuffer(GL_NONE);
	if(apiCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) logError("Framebuffer is not complete!");
}

Vector<UInt> Framebuffer::readPixelsAtCenterUIntRgb(const UInt attachment) const
{
	return readPixelsUIntRgb(size.x / 2, size.y / 2, attachment);
}

Vector<UInt> Framebuffer::readPixelsUIntRgb(const UInt x, const UInt y, const UInt attachment) const
{
	//read();
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferId);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment);
	Vector<UInt> data = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; //[TODO] check why i did this
	glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, data.data());
	return data;
}

//framebuffer texture
FramebufferTexture::FramebufferTexture()
{
	destroy();
}

void FramebufferTexture::destroy()
{
	texture.unload();
}

void FramebufferTexture::resize(const Area area)
{
	texture.resize(area);
}
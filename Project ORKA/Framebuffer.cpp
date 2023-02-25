
#include "Framebuffer.hpp"
#include "Renderer.hpp"

//framebuffer
Float Framebuffer::aspectRatio()
{
	return Float(size.x) / Float(size.y);
}

void Framebuffer::read()
{
	apiBindReadFramebuffer(framebufferID);
};
void Framebuffer::draw()
{
	apiBindDrawFramebuffer(framebufferID);
	apiViewport(0, 0, size.x, size.y);
	apiScissor(0, 0, size.x, size.y);
}
void Framebuffer::destroy()
{
	apiDeleteFramebuffer(framebufferID); //doesent work. ask Nvidia
	frameBufferTextures.clear();
}
void Framebuffer::create(Area size)
{
	this->size = size;
	apiGenFramebuffer(framebufferID);

	//[TODO] actually make use of this for debugging
	String name = "Framebuffer";
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	glObjectLabel(GL_FRAMEBUFFER, framebufferID, name.size(), name.data());
}
void Framebuffer::blitFramebuffer()
{
	apiBindDrawFramebuffer(0);
	apiBindReadFramebuffer(framebufferID);
	apiBlitFramebuffer(size.x, size.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}
void Framebuffer::resize(Area resolution)
{
	//apply size
	if (size != resolution) {
		size = resolution;
		for (auto t : frameBufferTextures) t.resize(resolution);
	}
}
void Framebuffer::setAsTexture(Index slot)
{
	for (auto t : frameBufferTextures) {
		if (t.isColor) {
			t.texture.use(slot);
			return;
		};
	}
	logError("Framebuffer does not have renderable texture!");
}
void Framebuffer::add(UInt components, DataType type, UInt slot)
{
	//create texture and add it to the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	//apiBindDrawFramebuffer(framebufferID);
	FramebufferTexture t;
	t.texture.load(size, components, type);
	t.texture.attachTexture(slot);
	t.slot = slot;
	frameBufferTextures.pushBack(t);

	//set up framebuffer for drawing
	UInt a = 0;
	Vector<UInt> drawBuffers;
	for (auto t : frameBufferTextures) {
		if (t.isColor) drawBuffers.pushBack(GL_COLOR_ATTACHMENT0 + a++);
	}

	glDrawBuffers(drawBuffers.size(), drawBuffers.data());
	glReadBuffer(GL_NONE); // apparently fixes problems on older gpus
	if (apiCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) logError("Framebuffer is not complete!");
}

Vector<UInt> Framebuffer::readPixelsAtCenterUIntRGB(UInt attachment)
{
	return readPixelsUIntRGB(size.x / 2, size.y / 2, attachment);
}
Vector<UInt> Framebuffer::readPixelsUIntRGB(UInt x, UInt y, UInt attachment)
{
	//read();
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferID);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment);
	Vector<UInt> data = { 0,0,0,0,0,0,0,0,0,0,0,0,0}; //[TODO] check why i did this
	glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &data[0]);
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
void FramebufferTexture::resize(Area area)
{
	texture.resize(area);
}
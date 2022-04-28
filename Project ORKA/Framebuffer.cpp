
#include "Framebuffer.hpp"
#include "Renderer.hpp"
#include "InputManager.hpp"

Framebuffer::Framebuffer() {
	apiGenFramebuffer(framebufferID);
	apiBindFramebuffer(framebufferID);
}

Framebuffer::~Framebuffer() {
	apiDeleteFramebuffer(framebufferID);
}

void Framebuffer::use()
{
	apiBindFramebuffer(framebufferID);
}
void Framebuffer::destroy()
{
	apiBindFramebuffer(0);
	apiDeleteFramebuffer(framebufferID); //doesent work. ask Nvidia
	frameBufferTextures.clear();
}
Float Framebuffer::aspectRatio() {
	return Float(size.x) / Float(size.y);
}
void Framebuffer::blitFramebuffer()
{
	apiBindDrawFramebuffer(0);
	apiBindReadFramebuffer(framebufferID);
	apiBlitFramebuffer(size.x, size.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);
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
void Framebuffer::add(UInt components, DataType type, UInt slot) {
	//create texture and add it to the framebuffer
	apiBindFramebuffer(framebufferID);
	FramebufferTexture t;
	t.texture.load(size, components, type);
	t.texture.attachTexture(slot);
	t.slot = slot;
	frameBufferTextures.push_back(t);

	//set up framebuffer for drawing
	UInt a = 0;
	Vector<UInt> drawBuffers;
	for (auto t : frameBufferTextures) {
		if (t.isColor) drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + a++);
	}

	glDrawBuffers(drawBuffers.size(), drawBuffers.data());
	glReadBuffer(GL_NONE); // apparently fixes problems on older gpus
	if (apiCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) logError("Framebuffer is not complete!");
}
void Framebuffer::resize(Area resolution)
{
	//apply size
	size = resolution;

	for (auto t : frameBufferTextures) t.resize(size);
}

IDFrameBuffer::IDFrameBuffer() {
	add(3, dataTypeUInt, 0);
	add(5, dataTypeFloat, 1);
}

PixelIDs IDFrameBuffer::getID() {
	return getIDUnderCursor();
}

PixelIDs IDFrameBuffer::getIDUnderCursor() {
	if (inputManager.capturing) return getIDsAtCenter();
	else return getIDsAtLocation(inputManager.cursorPosition.x, inputManager.cursorPosition.y);
}

PixelIDs IDFrameBuffer::getIDsAtCenter()
{
	return getIDsAtLocation(size.x / 2, size.y / 2);
}

PixelIDs IDFrameBuffer::getIDsAtLocation(UInt x, UInt y)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferID);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	PixelIDs data;
	glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &data);

	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	return data;
}

FramebufferTexture::FramebufferTexture() {
	destroy();
}

void FramebufferTexture::destroy() {
	texture.unload();
}

void FramebufferTexture::resize(Area area) {
	texture.resize(area);
}

GBuffer::GBuffer() {
	add(4, dataTypeFloat, 0);
	add(3, dataTypeFloat, 1);
	add(3, dataTypeFloat, 2);
	add(1, dataTypeUInt, 3);
	add(5, dataTypeFloat, 4);
}

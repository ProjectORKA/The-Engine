
#include "Framebuffer.hpp"
#include "Renderer.hpp"

Float Framebuffer::aspectRatio() {
	return Float(size.x) / Float(size.y);
}

void Framebuffer::create()
{
	apiGenFramebuffer(framebufferID);
	apiBindFramebuffer(framebufferID);

	colorTexture.load(size, 4, dataTypeFloat);
	colorTexture.attachTexture(0);

	normalTexture.load(size, 3, dataTypeFloat);
	normalTexture.attachTexture(1);

	positionTexture.load(size, 3, dataTypeByte);
	positionTexture.attachTexture(2);

	materialIDTexture.load(size, 1, dataTypeUInt);
	materialIDTexture.attachTexture(3);

	depthTexture.load(size, 5, dataTypeFloat);
	depthTexture.attachTexture(0);

	UInt attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);
	glReadBuffer(GL_NONE); // apparently fixes problems on older gpus

	if (apiCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) logError("Framebuffer is not complete!");
}
void Framebuffer::destroy()
{
	apiBindFramebuffer(0);
	apiDeleteFramebuffer(framebufferID); //doesent work. ask Nvidia
	colorTexture.unload();
	normalTexture.unload();
	positionTexture.unload();
	materialIDTexture.unload();
	depthTexture.unload();
}
void Framebuffer::setAsTexture()
{
	colorTexture.use(0);
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
	size = resolution;

	//update textures
	colorTexture.resize(size);
	depthTexture.resize(size);
	normalTexture.resize(size);
	positionTexture.resize(size);
	materialIDTexture.resize(size);
}
void Framebuffer::use()
{
	apiBindFramebuffer(framebufferID);
}
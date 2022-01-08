
#include "Framebuffer.hpp"
#include "Renderer.hpp"

void Framebuffer::create()
{
	//generate framebuffer
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

	objectIDTexture.load(size, 1, dataTypeUInt);
	objectIDTexture.attachTexture(4);

	depthTexture.load(size, 5, dataTypeFloat);
	depthTexture.attachTexture(0);

	UInt attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(5, attachments);

	//check if correct
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
	objectIDTexture.unload();
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
	aspectRatio = Float(size.x) / Float(size.y);

	//update textures
	colorTexture.resize(size);
	normalTexture.resize(size);
	positionTexture.resize(size);
	materialIDTexture.resize(size);
	objectIDTexture.resize(size);
	depthTexture.resize(size);
}
void Framebuffer::use(Renderer & renderer)
{
	apiBindFramebuffer(framebufferID);
	renderer.uniforms().width() = size.x;
	renderer.uniforms().height() = size.y;
}
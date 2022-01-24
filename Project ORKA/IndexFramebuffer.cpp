
#include "IndexFramebuffer.hpp"

void IDFrameBuffer::use() {
	apiBindFramebuffer(framebufferID);
}
void IDFrameBuffer::destroy() {
	apiBindFramebuffer(0);
	apiDeleteFramebuffer(framebufferID); //doesent work. ask Nvidia
	idTexture.unload();
	depthTexture.unload();
}
Float IDFrameBuffer::aspectRatio() {
	return Float(size.x) / Float(size.y);
}
void IDFrameBuffer::create(Area size) {
	this->size = size;
	apiGenFramebuffer(framebufferID);
	apiBindFramebuffer(framebufferID);

	idTexture.load(size, 3, dataTypeUInt);
	idTexture.attachTexture(0);

	depthTexture.load(size, 5, dataTypeFloat);
	depthTexture.attachTexture(0);

	UInt attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);
	glReadBuffer(GL_NONE); // apparently fixes problems on older gpus
	if (apiCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) logError("Framebuffer is not complete!");
}
void IDFrameBuffer::resize(Area resolution) {
	//apply size
	size = resolution;

	//update textures
	idTexture.resize(size);
	depthTexture.resize(size);
}
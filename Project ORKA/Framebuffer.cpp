#include "Framebuffer.hpp"
#include "Renderer.hpp"

//framebuffer
Float Framebuffer::aspectRatio() const { return static_cast<Float>(size.x) / static_cast<Float>(size.y); }

void Framebuffer::read() const { apiBindReadFramebuffer(framebufferID); };

void Framebuffer::draw() const {
	apiBindDrawFramebuffer(framebufferID);
	apiViewport(0, 0, size.x, size.y);
	apiScissor(0, 0, size.x, size.y);
}

void Framebuffer::destroy() {
	apiDeleteFramebuffer(framebufferID); //doesent work. ask Nvidia
	frameBufferTextures.clear();
}

void Framebuffer::create(const Area size) {
	this->size = size;
	apiGenFramebuffer(framebufferID);

	//[TODO] actually make use of this for debugging
	const String name = "Framebuffer";
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	glObjectLabel(GL_FRAMEBUFFER, framebufferID, name.size(), name.data());
}

void Framebuffer::blitFramebuffer() const {
	apiBindDrawFramebuffer(0);
	apiBindReadFramebuffer(framebufferID);
	apiBlitFramebuffer(size.x, size.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void Framebuffer::resize(const Area resolution) {
	//apply size
	if (size != resolution) {
		size = resolution;
		for (auto t : frameBufferTextures) t.resize(resolution);
	}
}

void Framebuffer::setAsTexture(const Index slot) const {
	for (const auto t : frameBufferTextures) {
		if (t.isColor) {
			t.texture.use(slot);
			return;
		}
	}
	logError("Framebuffer does not have renderable texture!");
}

void Framebuffer::add(const UInt components, const DataType type, const UInt slot) {
	//create texture and add it to the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	//apiBindDrawFramebuffer(framebufferID);
	FramebufferTexture t;
	t.texture.load(size, components, type);
	t.texture.attachTexture(slot);
	t.slot = slot;
	frameBufferTextures.push_back(t);

	//set up framebuffer for drawing
	UInt a = 0;
	Vector<UInt> drawBuffers;
	for (const auto t : frameBufferTextures) { if (t.isColor) drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + a++); }

	glDrawBuffers(drawBuffers.size(), drawBuffers.data());
	glReadBuffer(GL_NONE); // apparently fixes problems on older gpus
	if (apiCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) logError("Framebuffer is not complete!");
}

Vector<UInt> Framebuffer::readPixelsAtCenterUIntRGB(const UInt attachment) {
	return readPixelsUIntRGB(size.x / 2, size.y / 2, attachment);
}

Vector<UInt> Framebuffer::readPixelsUIntRGB(const UInt x, const UInt y, const UInt attachment) const {
	//read();
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferID);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment);
	Vector<UInt> data = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //[TODO] check why i did this
	glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &data[0]);
	return data;
}

//framebuffer texture
FramebufferTexture::FramebufferTexture() { destroy(); }

void FramebufferTexture::destroy() { texture.unload(); }
void FramebufferTexture::resize(const Area area) { texture.resize(area); }

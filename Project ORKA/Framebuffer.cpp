
#include "Framebuffer.hpp"
#include "Renderer.hpp"

void Framebuffer::use(Renderer & renderer)
{
	apiBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	renderer.uniforms().data.width = size.x;
	renderer.uniforms().data.height = size.y;
}
void Framebuffer::create()
{
	//generate framebuffer
	apiGenFramebuffer(framebufferID);

	//generate color texture
	CPUTexture color;
	color.name = "postProcess";
	color.width = size.x;
	color.height = size.y;
	color.channels = 4;
	color.dataType = dataTypeByte;
	color.pixels = nullptr;
	color.nearFilter = Filter::nearest;
	color.farFilter = Filter::nearest;
	color.wrapping = clamped;
	color.loaded = true;

	//upload to GPU
	colorTexture.load(color);

	//attach to Framebuffer
	attachTexture(colorTexture);

	//generate depth texture
	CPUTexture depth;
	depth.name = "depthTexture";
	depth.width = size.x;
	depth.height = size.y;
	depth.channels = 5;
	depth.dataType = dataTypeFloat;
	depth.pixels = nullptr;
	color.nearFilter = Filter::nearest;
	color.farFilter = Filter::nearest;
	depth.wrapping = border;
	depth.loaded = true;

	//upload to GPU
	depthTexture.load(depth);

	attachTexture(depthTexture);

	//apiNamedFramebufferDrawBuffer(framebufferID); //[TODO] check if unnecessary

	//check if correct
	if (apiCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) logError("Framebuffer is not complete!");
}
void Framebuffer::destroy()
{
	apiBindFramebuffer(GL_FRAMEBUFFER, 0);
	//apiDrawBuffer(GL_BACK); //[TODO] check if unnecessary
	apiDeleteFramebuffer(framebufferID); //doesent work. ask Nvidia
	colorTexture.unload();
	depthTexture.unload();
}
void Framebuffer::setAsTexture()
{
	colorTexture.use(0);
}
void Framebuffer::detachTextures() {
	apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
	apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
}
void Framebuffer::blitFramebuffer()
{
	apiBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	apiBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferID);
	apiBlitFramebuffer(size.x, size.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}
void Framebuffer::resize(Area resolution)
{
	//apply size
	size = resolution;
	aspectRatio = Float(size.x) / Float(size.y);

	//update textures
	colorTexture.resize(size);
	depthTexture.resize(size);
}
void Framebuffer::attachTexture(GPUTexture& texture)
{
	apiBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

	if (texture.channels < 5) {
		//its a color texture
		apiBindTexture(GL_TEXTURE_2D, texture.textureID);
		apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.textureID, 0);
	}
	else {
		//its a depth texture
		apiBindTexture(GL_TEXTURE_2D, texture.textureID);
		apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture.textureID, 0);
	}
}
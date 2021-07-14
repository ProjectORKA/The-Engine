
#include "Framebuffer.hpp"

void Framebuffer::use()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	glNamedFramebufferDrawBuffer(framebufferID, GL_COLOR_ATTACHMENT0);
}
void Framebuffer::create()
{
	//generate framebuffer
	glGenFramebuffers(1, &framebufferID);

	//generate color texture
	CPUTexture color;
	color.name = "postProcess";
	color.width = width;
	color.height = height;
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
	depth.width = width;
	depth.height = height;
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

	glNamedFramebufferDrawBuffer(framebufferID, GL_COLOR_ATTACHMENT0);

	//check if correct
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) logError("Framebuffer is not complete!");
}
void Framebuffer::destroy()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);
	glDeleteFramebuffers(1, &framebufferID); //doesent work. ask Nvidia
	colorTexture.unload();
	depthTexture.unload();
}
void Framebuffer::setAsTexture()
{
	colorTexture.use(0);
}
void Framebuffer::detachTextures() {
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
}
void Framebuffer::blitFramebuffer()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferID);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}
void Framebuffer::resize(Vec2 resolution)
{

	//needs to be at least 1 x 1
	resolution.x = max<Float>(1.0f, resolution.x);
	resolution.y = max<Float>(1.0f, resolution.y);

	//if size actually changed
	if ((width != resolution.x) | (height != resolution.y)) {

		//apply size
		width = resolution.x;
		height = resolution.y;
		aspectRatio = Float(width) / Float(height);

		//update textures
		colorTexture.resize(width, height);
		depthTexture.resize(width, height);
	}
}
void Framebuffer::attachTexture(GPUTexture& texture)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

	if (texture.channels < 5) {
		//its a color texture
		glBindTexture(GL_TEXTURE_2D, texture.textureID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.textureID, 0);
	}
	else {
		//its a depth texture
		glBindTexture(GL_TEXTURE_2D, texture.textureID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture.textureID, 0);
	}
}
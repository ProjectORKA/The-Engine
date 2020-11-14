
#include "Framebuffer.hpp"

//framebuffer
void Framebuffer::use()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	glNamedFramebufferDrawBuffer(framebufferID, GL_COLOR_ATTACHMENT0);
}
void Framebuffer::render()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferID);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}
void Framebuffer::create()
{
	//generate framebuffer
	glGenFramebuffers(1, &framebufferID);

	//generate color texture
	CPUTexture color;
	color.name = "postProcessTexture";
	color.width = width;
	color.height = height;
	color.channels = 4;
	color.dataType = dataTypeByte;
	color.pixels = nullptr;
	color.filter = nearest;
	color.wrapping = clamped;
	color.loaded = true;
	color.multisampling = samples;

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
	depth.filter = nearest;
	depth.wrapping = clamped;
	depth.loaded = true;
	depth.multisampling = samples;

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
void Framebuffer::detachTextures() {
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, 0, 0);
}
void Framebuffer::resize(Vec2& resolution)
{
	//needs to be at least 1 x 1
	resolution.x = max(1, resolution.x);
	resolution.y = max(1, resolution.y);

	//if size actually changed
	if ((width != resolution.x) | (height != resolution.y)) {

		//apply size
		width = resolution.x;
		height = resolution.y;

		//update textures
		colorTexture.resize(width, height);
		depthTexture.resize(width, height);
	}
}
void Framebuffer::changeSamples(UShort samples) {
	destroy();
	this->samples = samples;
	create();
}
void Framebuffer::attachTexture(GPUTexture& texture)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

	if (texture.channels < 5) {
		//its a color texture
		if (texture.multisampling > 0) {
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture.textureID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture.textureID, 0);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, texture.textureID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.textureID, 0);
		}
	}
	else {
		//its a depth texture
		if (texture.multisampling > 0) {
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture.textureID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, texture.textureID, 0);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, texture.textureID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture.textureID, 0);
		}
	}
}

//FramebufferSystem
void FramebufferSystem::create()
{
	framebuffers.clear();
	add(adaptiveResolution, 0);
	add(adaptiveResolution, 2);
	add(adaptiveResolution, 4);
	add(adaptiveResolution, 8);
	add(adaptiveResolution, 16);
}
void FramebufferSystem::destroy()
{
	deselect();
	for (Framebuffer& framebuffer : framebuffers) {
		framebuffer.destroy();
	}
	framebuffers.clear();
}
void FramebufferSystem::deselect()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);
}
void FramebufferSystem::updateFramebufferSizes()
{
	for (Framebuffer& framebuffer : framebuffers) {
		framebuffer.resize(adaptiveResolution);
	}
}
void FramebufferSystem::select(Index framebufferIndex)
{
	if (framebufferIndex > (framebuffers.size() - 1)) {
		logError(String("Invalid framebufferIndex! (").append(std::to_string(framebufferIndex)).append(")"));
		return;
	}
	currentFramebufferIndex = framebufferIndex;
	current().use();
}
void FramebufferSystem::add(Vec2 resolution, UShort samples)
{
	framebuffers.emplace_back();
	framebuffers.back().samples = samples;
	framebuffers.back().create();
	framebuffers.back().resize(resolution);
	select(framebuffers.size() - 1);
}

Framebuffer& FramebufferSystem::current()
{
	if ((currentFramebufferIndex > (framebuffers.size() - 1)) | (framebuffers.size() == 0)) {
		logError(String("Invalid framebufferIndex! (").append(std::to_string(currentFramebufferIndex)).append(")"));
	}
	return framebuffers[currentFramebufferIndex];
}
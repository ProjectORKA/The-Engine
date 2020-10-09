
#include "Framebuffer.hpp"

void Framebuffer::create(TextureSystem& textureSystem)
{
	//generate framebuffer
	glGenFramebuffers(1, &framebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

	//generate color texture
	CPUTexture color;
	color.name = "postProcessTexture";
	color.width = 1600;
	color.height = 900;
	color.channels = 4;
	color.dataType = dataTypeByte;
	color.pixels = nullptr;
	color.filter = nearest;
	color.wrapping = clamped;
	color.loaded = true;
	textureSystem.add(color);
	colorTextureIndex = textureSystem.currentIndex;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureSystem.current().textureID, 0);

	//generate depth texture
	CPUTexture depth;
	depth.name = "depthTexture";
	depth.width = 1600;
	depth.height = 900;
	depth.channels = 5;
	depth.dataType = dataTypeByte;
	depth.pixels = nullptr;
	depth.filter = nearest;
	depth.wrapping = clamped;
	depth.loaded = true;
	textureSystem.add(depth);
	depthTextureIndex = textureSystem.currentIndex;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureSystem.current().textureID, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
	
	//check if correct
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) logError("Framebuffer is not complete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::update(TextureSystem& textureSystem, Vec2& resolution)
{
	resolution.x = max(1, resolution.x);
	resolution.y = max(1, resolution.y);

	if ((width != resolution.x) | (height != resolution.y)) {
		logEvent("Framebuffer needs resize!");
		width = resolution.x;
		height = resolution.y;

		textureSystem.use(colorTextureIndex);
		textureSystem.resize(width, height);

		textureSystem.use(depthTextureIndex);
		textureSystem.resize(width, height);

		//glBindRenderbuffer(GL_RENDERBUFFER, depthTextureIndex);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	}
}

void Framebuffer::use()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);
}

void Framebuffer::clear()
{
	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Framebuffer::destroy()
{
	glDeleteFramebuffers(1, &framebufferID);
}

void FramebufferSystem::create(RenderObjectSystem & renderObjectSystem)
{
	framebuffers.clear();
	add(adaptiveResolution, renderObjectSystem.textureSystem);
	select(0);

	//essential objects
	renderObjectSystem.addRenderObject("postProcess", "postProcessPlane", "postProcessTexture", "postProcess");
}

void FramebufferSystem::add(Vec2 resolution, TextureSystem & textureSystem)
{
	framebuffers.emplace_back();
	framebuffers.back().create(textureSystem);
	framebuffers.back().update(textureSystem, resolution);
}

void FramebufferSystem::updateFramebuffers(TextureSystem & textureSystem)
{
	for (Framebuffer& framebuffer : framebuffers) {
		framebuffer.update(textureSystem, adaptiveResolution);
	}
}

void FramebufferSystem::select(Index framebufferIndex)
{
	if (framebufferIndex > (framebuffers.size() - 1)) {
		logError(String("Invalid framebufferIndex! (" ).append(std::to_string(framebufferIndex)).append(")"));
		return;
	}
	currentFramebufferIndex = framebufferIndex;
	current().use();
}

void FramebufferSystem::selectFinal()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer& FramebufferSystem::current()
{
	if ((currentFramebufferIndex > (framebuffers.size() - 1)) | (framebuffers.size() == 0)) {
		logError(String("Invalid framebufferIndex! (").append(std::to_string(currentFramebufferIndex)).append(")"));
	}
	return framebuffers[currentFramebufferIndex];
}
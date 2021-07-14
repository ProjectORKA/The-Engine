#include "Renderer.hpp"

void Renderer::end()
{
	pollGraphicsAPIError();
	mutex.unlock();
	frameCount++;
}
void Renderer::sync()
{
	mutex.lock();
	mutex.unlock();
}
void Renderer::begin()
{
	mutex.lock();
	renderTime.update();
	framebufferSystem.update();
	framebufferSystem.select(0);

	clearColor();
	clearDepth();
}
void Renderer::create()
{
	//basic systems
	renderTime.reset();
	cameraSystem.create();
	framebufferSystem.create();
	textureSystem.create();
	meshSystem.create();
	shaderSystem.create();

	//advanced systems
	textRenderSystem.create(*this);
	planetRenderSystem.create(*this);

	renderObjectSystem.create(*this);

	glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
	glEnable(GL_SCISSOR_TEST);
}
void Renderer::destroy()
{
	textRenderSystem.destroy();
	planetRenderSystem.destroy();
	renderObjectSystem.destroy();

	meshSystem.destroy();
	shaderSystem.destroy();
	textureSystem.destroy();

	cameraSystem.destroy();
	framebufferSystem.destroy();
}
void Renderer::clearDepth() {
	glClear(GL_DEPTH_BUFFER_BIT);
}
void Renderer::clearColor()
{
	glClear(GL_COLOR_BUFFER_BIT);
}
void Renderer::createBlurTexture(Framebuffer & from, Framebuffer & to)
{
	//this function renders a blurred version of a framebuffer to another framebuffer
	Index originalFramebufferID = framebufferSystem.currentFramebufferIndex;
	
	shaderSystem.use("blur");
	from.colorTexture.use(0);
	to.use();
	meshSystem.renderMesh("fullScreenQuad");

	framebufferSystem.select(originalFramebufferID);
}

void Renderer::pollGraphicsAPIError() {
	GLenum error = glGetError();
	if (error) {
		logError(String("Opengl Error: ").append(std::to_string(error)));
	}
}
void Renderer::clearColor(Color color) {
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT);
}
void Renderer::setCulling(Bool isCulling) {
	if (isCulling) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	else {
		glDisable(GL_CULL_FACE);
	}
}
void Renderer::setDepthClamp(Bool depthClamp)
{
	if (depthClamp) {
		glEnable(GL_DEPTH_CLAMP);
	}
	else {
		glDisable(GL_DEPTH_CLAMP);
	}
}
void Renderer::setAlphaBlending(Bool blending)
{
	if (blending) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_DST_ALPHA);
		glBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);
	}
	else glDisable(GL_BLEND);
}
void Renderer::setDepthTest(Bool isUsingDepth) {
	if (isUsingDepth) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}
}
void Renderer::setWireframeMode(Bool wireframeMode) {
	if (wireframeMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}
void Renderer::addRenderObject(RenderObjectNames renderObjectNames)
{
	renderObjectSystem.addRenderObject(renderObjectNames);
}

Float& Renderer::aspectRatio()
{
	return framebufferSystem.current().aspectRatio;
}
Uniforms& Renderer::uniforms()
{
	return shaderSystem.uniforms;
}

void loadPrimitives(Renderer& renderer) {
	renderer.renderObjectSystem.addRenderObject("circle", "circle", "default", "basicColor");
}

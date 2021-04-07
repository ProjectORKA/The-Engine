#include "Renderer.hpp"

void Renderer::end()
{
	pollGraphicsAPIError();
	mutex.unlock();
}
void Renderer::begin()
{
	mutex.lock();
	renderTime.update();
	framebufferSystem.update();
	
	clearColor();
	clearDepth();

	switch (multisampleCount) {
	case 0:  framebufferSystem.select(0); break;
	case 2:  framebufferSystem.select(1); break;
	case 4:  framebufferSystem.select(2); break;
	case 8:  framebufferSystem.select(3); break;
	case 16: framebufferSystem.select(4); break;
	default: framebufferSystem.select(0);
	}

	clearColor();
	clearDepth();
}
void Renderer::create()
{
	renderTime.reset();
	cameraSystem.create();
	viewportSystem.create();
	framebufferSystem.create();
	meshSystem.create();
	textureSystem.create();
	shaderSystem.create();

	//advanced systems
	renderObjectSystem.create(meshSystem,textureSystem,shaderSystem);
	//textRenderSystem.create(textureSystem);
	//sprite renderer
	//particle system
	//instanced renderer

	glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
}
void Renderer::destroy()
{
	planetRenderSystem.destroy();
	renderObjectSystem.destroy();

	meshSystem.destroy();
	shaderSystem.destroy();
	textureSystem.destroy();

	cameraSystem.destroy();
	viewportSystem.destroy();
	framebufferSystem.destroy();
}
void Renderer::clearDepth() {
	glClear(GL_DEPTH_BUFFER_BIT);
}
void Renderer::clearColor()
{
	glClear(GL_COLOR_BUFFER_BIT);
}
void Renderer::updateUniforms() {
	uniforms().setMatrix("mMatrix", Matrix(1));
}
void Renderer::resetModelMatrix()
{
	uniforms().setMatrix("mMatrix", Matrix(1));
}
void Renderer::renderFramebuffer()
{
	framebufferSystem.deselect();
	setWireframeMode(false);
	uniforms().setInt("sampleCount", Int(currentFramebuffer().colorTexture.sampleCount));
	shaderSystem.use("postProcess");
	currentFramebuffer().colorTexture.use();
	meshSystem.renderMesh("fullScreenQuad");
}
void Renderer::pollGraphicsAPIError() {
	GLenum error = glGetError();
	if (error) {
		std::cout << "OpenGl Error: " << error << "\n";
	}
}
void Renderer::waitForFinishedFrame()
{
	mutex.lock();
	mutex.unlock();
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
		//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_DST_ALPHA);
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

Float& Renderer::aspectRatio()
{
	return framebufferSystem.current().aspectRatio;
}
Uniforms& Renderer::uniforms()
{
	return shaderSystem.uniforms;
}
Viewport& Renderer::currentViewport()
{
	return viewportSystem.current();
}
Framebuffer& Renderer::currentFramebuffer()
{
	return framebufferSystem.current();
}
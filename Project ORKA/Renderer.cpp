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
	mutex.lock();											//used for syncronizing other threads
	renderTime.update();									//advances the time
	shaderSystem.uniforms.data.time = renderTime.total;		//makes time available to shaders
	framebufferSystem.deselect();							//selects the backbuffer of the window
	clearColor(Color(Vec3(0), 0.0));						//and clears its contents
	clearDepth();
}
void Renderer::create()
{
	//basic systems
	renderTime.reset();
	textureSystem.create();
	meshSystem.create();
	shaderSystem.create();

	//advanced systems
	framebufferSystem.create(*this);
	textRenderSystem.create(*this);
	renderObjectSystem.create(*this);
	planetRenderSystem.create(*this);

	apiClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
	apiEnable(GL_SCISSOR_TEST);
}
void Renderer::destroy()
{
	textRenderSystem.destroy();
	planetRenderSystem.destroy();
	renderObjectSystem.destroy();

	meshSystem.destroy();
	shaderSystem.destroy();
	textureSystem.destroy();

	framebufferSystem.destroy();
}
void Renderer::clearDepth() {
	apiClearDepth();
}
void Renderer::clearColor()
{
	apiClearColor();
}
void Renderer::screenSpace() {
	Float width = renderRegion.region.size.x, height = renderRegion.region.size.y;
	Matrix matrix = Matrix(1);
	matrix = glm::translate(matrix, Vec3(-1, -1, 0));
	matrix = glm::scale(matrix, Vec3(2, 2, 0));
	matrix = glm::scale(matrix, Vec3(1.0 / width, 1.0 / height, 0));
	uniforms().data.vMatrix = Matrix(1);
	uniforms().data.pMatrix = matrix;
}
void Renderer::normalizedSpace() {
	uniforms().data.vMatrix = Matrix(1);
	uniforms().data.pMatrix = Matrix(1);
}
void Renderer::setWireframeMode() {
	setWireframeMode(wireframeMode);
}
void Renderer::renderMesh(Name name) {
	uniforms().update();
	meshSystem.render(name);
}
void Renderer::pollGraphicsAPIError() {
	Int error = apiGetError();
	if (error) {
		logError(String("Opengl Error: ").append(toString(error)));
	}
}
void Renderer::clearColor(Color color) {
	apiSetClearColor(color);
	apiClearColor();
}
void Renderer::setCulling(Bool isCulling) {
	if (isCulling) {
		apiEnable(GL_CULL_FACE);
		apiCullFace(GL_BACK);
	}
	else {
		apiDisable(GL_CULL_FACE);
	}
}
void Renderer::apectCorrectNormalizedSpace() {
	Float aspect = aspectRatio();
	uniforms().data.vMatrix = Matrix(1);
	if (aspect > 1) uniforms().data.pMatrix = scale(Matrix(1), Vec3(1 / aspectRatio(), 1, 1));
	else uniforms().data.pMatrix = scale(Matrix(1), Vec3(1, aspectRatio(), 1));
}
void Renderer::setDepthClamp(Bool depthClamp)
{
	if (depthClamp) {
		apiEnable(GL_DEPTH_CLAMP);
	}
	else {
		apiDisable(GL_DEPTH_CLAMP);
	}
}
void Renderer::setAlphaBlending(Bool blending)
{
	if (blending) {
		apiEnable(GL_BLEND);
		apiBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		apiBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_DST_ALPHA);
		apiBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);
	}
	else apiDisable(GL_BLEND);
}
void Renderer::setDepthTest(Bool isUsingDepth) {
	if (isUsingDepth) {
		apiEnable(GL_DEPTH_TEST);
		apiDepthFunc(GL_LEQUAL);
	}
	else {
		apiDisable(GL_DEPTH_TEST);
	}
}
void Renderer::setWireframeMode(Bool wireframeMode) {
	if (wireframeMode) {
		apiPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		apiPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}
void Renderer::createBlurTexture(Index from, Index to)
{
	//this function renders a blurred version of a framebuffer to another framebuffer
	Index originalFramebufferID = framebufferSystem.currentFramebufferIndex;
	
	useShader("blur");
	framebufferSystem.framebuffers[from].colorTexture.use(0);
	framebufferSystem.use(*this, to);
	renderMesh("fullScreenQuad");

	framebufferSystem.use(*this, originalFramebufferID);
}
void Renderer::addRenderObject(RenderObjectNames renderObjectNames)
{
	renderObjectSystem.addRenderObject(renderObjectNames);
}

Bool Renderer::getCulling() {
	return apiGetCullFace();
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

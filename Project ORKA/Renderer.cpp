#include "Renderer.hpp"

void Renderer::sync()
{
	mutex.lock();
	mutex.unlock();
}
void Renderer::create()
{
	renderTime.reset();
	cameraSystem.create();
	viewportSystem.create();
	renderObjectSystem.create();
	framebufferSystem.create();
	planetRenderSystem.create();

	glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

	//cameraSystem.current().speedMultiplier = 1;
}
void Renderer::render()
{
	mutex.lock();
	renderTime.update();
	dynamicallyAdjustValue(*this, planetRenderSystem.renderDistance);

	//logDebug(planetRenderSystem.renderDistance);

	////////////////////////////////////////////////////////////////////////

	planetRenderSystem.update(gameSimulation->planetSystem.chunk, currentCamera());

	//camera transform is immediate and handled by render client for best performance
	currentCamera().update(renderTime);

	//resizes all framebuffers if necessary
	framebufferSystem.updateFramebufferSizes();

	switch (multisampleCount) {
	case 0:  framebufferSystem.select(0); break;
	case 2:  framebufferSystem.select(1); break;
	case 4:  framebufferSystem.select(2); break;
	case 8:  framebufferSystem.select(3); break;
	case 16: framebufferSystem.select(4); break;
	default: framebufferSystem.select(0);
	}

	clearColor(Color(0, 0, 0, 0.75));

	//enables or disables wireframe mode
	updateWireframeMode(wireframeMode);

	viewportSystem.select("full");
	viewportSystem.render(framebufferSystem.adaptiveResolution.x, framebufferSystem.adaptiveResolution.y);


	//render sky
	culling(false);
	depthTest(false);
	resetModelMatrix();
	uniforms().setVec3("chunkOffsetVector", Vec3(0));
	uniforms().setVec3("cameraVector", cameraSystem.current().forwardVector);
	uniforms().setMatrix("vpMatrix", cameraSystem.current().projectionMatrix(viewportSystem.current().aspectRatio()) * cameraSystem.current().viewMatrixOnlyRot());
	renderObjectSystem.render("sky");

	//setup actual scene
	culling(true);
	clearDepth();
	depthTest(true);

	currentCamera().render(*this);

	//render planet
	currentCamera().render(*this);
	planetRenderSystem.render(uniforms(), renderObjectSystem);

	//put rendered image in main framebuffer
	currentFramebuffer().render();

	//////////////////////////////////////////////////////////////////
	glFinish();

	pollGraphicsAPIError();

	mutex.unlock();
}
void Renderer::destroy()
{
	viewportSystem.destroy();
	cameraSystem.destroy();
	renderObjectSystem.destroy();
}
void Renderer::clearDepth() {
#ifdef GRAPHICS_API_OPENGL
	glClear(GL_DEPTH_BUFFER_BIT);
#endif // GRAPHICS_API_OPENGL
}
void Renderer::updateUniforms() {
	uniforms().setMatrix("mMatrix", Matrix(1));
}
void Renderer::resetModelMatrix()
{
	uniforms().setMatrix("mMatrix", Matrix(1));
}
void Renderer::clearColor(Color color) {
#ifdef GRAPHICS_API_OPENGL
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT);
#endif // GRAPHICS_API_OPENGL
}

Uniforms& Renderer::uniforms()
{
	return renderObjectSystem.shaderSystem.uniforms;
}
Camera& Renderer::currentCamera()
{
	return cameraSystem.current();
}
Viewport& Renderer::currentViewport()
{
	return viewportSystem.current();
}
Framebuffer& Renderer::currentFramebuffer()
{
	return framebufferSystem.current();
}

void culling(bool isCulling) {
	if (isCulling) {
#ifdef GRAPHICS_API_OPENGL
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
#endif // GRAPHICS_API_OPENGL
	}
	else {
#ifdef GRAPHICS_API_OPENGL
		glDisable(GL_CULL_FACE);
#endif // GRAPHICS_API_OPENGL
	}
}
void depthTest(bool isUsingDepth) {
	if (isUsingDepth) {
#ifdef GRAPHICS_API_OPENGL
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
#endif // GRAPHICS_API_OPENGL
	}
	else {
#ifdef GRAPHICS_API_OPENGL
		glDisable(GL_DEPTH_TEST);
#endif // GRAPHICS_API_OPENGL
	}
}
void updateWireframeMode(bool wireframeMode) {
	if (wireframeMode) {
#ifdef GRAPHICS_API_OPENGL
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif // GRAPHICS_API_OPENGL
	}
	else {
#ifdef GRAPHICS_API_OPENGL
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif // GRAPHICS_API_OPENGL
	}
}

void pollGraphicsAPIError() {
#ifdef GRAPHICS_API_OPENGL
	GLenum error = glGetError();
	if (error) {
		std::cout << "OpenGl Error: " << error << "\n";
	}
#endif
}
void dynamicallyAdjustValue(Renderer& renderer, Float& value) {
	if (renderer.adjustRenderVariables) {
		//average fps, to avoid loopback
		static float averageDelta = 0.16;
		float oldToNewRatio = renderer.renderTime.delta / averageDelta; // >1 means framerate dropped
		
		//the code below gradually introduces the new values
		averageDelta += oldToNewRatio * renderer.renderTime.delta;
		averageDelta /= (1 + oldToNewRatio);

		logDebug(1.0f/averageDelta);
		logDebug(value);
		//get current fps relative to target
		//>1 means too low fps <1 means too high
		static float timePerFrameRatio = 1;
		timePerFrameRatio = averageDelta * renderer.targetFrameRate;

		if ((timePerFrameRatio > 1.1) | (timePerFrameRatio < 0.9)) {
			float lerpFactor = renderer.renderTime.delta / 2;
			value += lerpFactor * (value / timePerFrameRatio);
			value /= 1 + lerpFactor;
		}

		value = max(0.1, value);
	}
}
void renderSpaceShip(Renderer& renderer, SpaceShip& spaceShip)
{
	spaceShip.rotation = getRotationBetweenVectors(Vec3(0, 0, 1), spaceShip.velocity);
	renderer.uniforms().setMatrix("mMatrix", glm::translate(Matrix(1), spaceShip.location) * glm::toMat4(spaceShip.rotation) * glm::scale(Matrix(1), Vec3(2)));
	//renderer.renderObjectSystem.render("spaceShip");
	renderer.renderObjectSystem.render("spaceShipLOD");
}
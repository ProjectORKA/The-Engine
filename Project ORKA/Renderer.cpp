#include "Renderer.hpp"

void Renderer::create()
{
	renderTime.reset();

	cameraSystem.create();
	viewportSystem.create();
	renderObjectSystem.create();
}
void Renderer::renderTest() {

};
void Renderer::render()
{
	mutex.lock();

	renderTime.update();
	dynamicallyAdjustValue(*this, settings.worldSystemRenderDistance);

	cameraSystem.current().processLocation(renderTime);
	Camera& camera = cameraSystem.current();

	//begin changing framebuffer
	clearColor(Color(0, 0, 0, 0.75));

	culling(false);
	depthTest(false);
	updateWireframeMode(wireframeMode);

	viewportSystem.select("left");
	viewportSystem.render(framebuffer);

	renderObjectSystem.shaderSystem.uniforms.matrices["mMatrix"] = Matrix(1);


	renderObjectSystem.shaderSystem.uniforms.matrices["vpMatrix"] = camera.projectionMatrix(viewportSystem.current().aspectRatio()) * camera.viewMatrixOnlyRot();
	renderObjectSystem.render("sky");

	culling(true);
	depthTest(true);
	clearDepth();

	renderObjectSystem.shaderSystem.uniforms.matrices["vpMatrix"] = camera.projectionMatrix(viewportSystem.current().aspectRatio()) * camera.viewMatrix();
	renderObjectSystem.shaderSystem.uniforms.matrices["mMatrix"] = glm::rotate(Matrix(1),gameSimulation->gameTime.total, Vec3(0, 0, 1));
	renderObjectSystem.render("default");
	renderObjectSystem.render("monkey");
	renderObjectSystem.render("skeleton");
	//////////////////////

	culling(false);
	depthTest(false);
	updateWireframeMode(true);

	viewportSystem.select("right");
	viewportSystem.render(framebuffer);

	renderObjectSystem.shaderSystem.uniforms.matrices["mMatrix"] = Matrix(1);
	renderObjectSystem.shaderSystem.uniforms.matrices["vpMatrix"] = camera.projectionMatrix(viewportSystem.current().aspectRatio()) * camera.viewMatrixOnlyRot();
	renderObjectSystem.render("sky");

	culling(true);
	depthTest(true);
	clearDepth();

	renderObjectSystem.shaderSystem.uniforms.matrices["vpMatrix"] = camera.projectionMatrix(viewportSystem.current().aspectRatio()) * camera.viewMatrix();
	renderObjectSystem.shaderSystem.uniforms.matrices["mMatrix"] = glm::rotate(Matrix(1), gameSimulation->gameTime.total, Vec3(0, 0, 1));
	renderObjectSystem.render("default");
	renderObjectSystem.render("monkey");
	renderObjectSystem.render("skeleton");

	/////////////////
	pollGraphicsAPIError();

	mutex.unlock();
}
void Renderer::sync()
{
	mutex.lock();
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
void Renderer::clearColor(Color color) {
#ifdef GRAPHICS_API_OPENGL
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT);
#endif // GRAPHICS_API_OPENGL
}
void Renderer::updateUniforms() {
	renderObjectSystem.shaderSystem.uniforms.matrices["mMatrix"] = Matrix(1);
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
void dynamicallyAdjustValue(Renderer& renderer, unsigned int& value) {
	if (renderer.settings.adjustRenderVariables) {
		float minimumFrameTimeMatchup = renderer.renderTime.delta * renderer.settings.minimumFrameRate;
		float maximumFrameTimeMatchup = renderer.renderTime.delta * renderer.settings.maximumFrameRate;
		if (minimumFrameTimeMatchup > 1) {
			if (value != 0) {
				value = (unsigned int)((float)value / DYNAMIC_VIEW_DISTANCE_MULTIPLIER);
			}
		}
		else {
			if (maximumFrameTimeMatchup < 1) {
				value++;
			}
		}
	}
}
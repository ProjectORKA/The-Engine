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
	renderTime.update();
	dynamicallyAdjustValue(*this, settings.worldSystemRenderDistance);

	cameraSystem.current().processLocation(renderTime);

	culling(true);
	depthTest(true);
	updateWireframeMode(wireframeMode);

	viewportSystem.select("full");
	viewportSystem.render(framebuffer);

	//begin changing framebuffer
	clearDepth();
	clearColor(Color(0, 0, 0, 0.75));

	updateUniforms();

	renderObjectSystem.render("defaultObject");

	//pollGraphicsAPIError();

	renderSync = true;
}
void Renderer::sync()
{
	renderSync = false;
	while (!renderSync) {};
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
	renderObjectSystem.shaderSystem.uniforms.floats["time"] = renderTime.total;
	renderObjectSystem.shaderSystem.uniforms.matrices["mMatrix"] = Matrix(1);
	//vp matrix
	Camera& camera = cameraSystem.current();
	Viewport& viewport = viewportSystem.current();
	renderObjectSystem.shaderSystem.uniforms.matrices["vpMatrix"] =
		glm::perspective(
			glm::radians(camera.fieldOfView),
			viewport.aspectRatio(),
			camera.nearClipValue,
			camera.farClipValue
		)
		*
		glm::lookAt(
			camera.location,
			camera.location + camera.forwardVector,
			camera.upVector
		);
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
void updateFramebuffer(Framebuffer& framebuffer, Int width, Int height) {
	framebuffer.size.x = width;
	framebuffer.size.y = height;
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
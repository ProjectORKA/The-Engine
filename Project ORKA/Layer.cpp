#include "Layer.hpp"
#include "CameraSystem.hpp"
#include "Program.hpp"

void Layer::create()
{
	if (createFunctionPtr) createFunctionPtr(*this);
}
void Layer::render()
{
	if (renderFunctionPtr) renderFunctionPtr(*this);
}
void Layer::destroy()
{
	if (destroyFunctionPtr) destroyFunctionPtr(*this);
}

Layer::Layer(Renderer& renderer, void(*createFunction)(Layer& layer), void(*renderFunction)(Layer& layer), void(*destroyFunction)(Layer& layer))
{
	rendererPtr = &renderer;
	this->createFunctionPtr = createFunction;
	this->renderFunctionPtr = renderFunction;
	this->destroyFunctionPtr = destroyFunction;

	time.pause();
	create();
	time.reset();
}

void createLogoLayer(Layer & layer)
{
	layer.rendererPtr->renderObjectSystem.addRenderObject("logo", "ProjectORKALogo", "ProjectORKABakedLogo", "unlit");
	layer.rendererPtr->renderObjectSystem.addRenderObject("mainMenu", "postProcessPlane", "MainMenuDesignPrototype", "unlit");
}
void renderLogoLayer(Layer& layer)
{
	layer.time.update();

	Renderer& renderer = *layer.rendererPtr;

	renderer.viewportSystem.select("full");
	renderer.viewportSystem.render(renderer.framebufferSystem.current());

	renderer.clearColor(Color(0, 0, 0, 0));
	renderer.clearDepth();

	renderer.setCulling(true);
	renderer.setDepthTest(true);
	renderer.setAlphaBlending(true);
	renderer.setWireframeMode(false);

	//camera stuff
	SimpleCamera c;
	c.setLocation(Vec3(0, -15, 0));
	renderer.uniforms().setMatrix("vpMatrix", c.projectionMatrix(renderer.aspectRatio()) * c.viewMatrix());

	//logo stuff
	Float animationLength = 1.1;
	Float size = min(pow(1 + 1 / pow(animationLength, 5), pow(layer.time.total - 2 * animationLength, 5)), 100.0);
	Float tint = max(2 - size, 0.0f);
	renderer.uniforms().setFloat("fade", 1);
	renderer.uniforms().setFloat("tint", tint);
	Matrix modelMatrix = glm::scale(Matrix(1), Vec3(size, 1, size));
	modelMatrix = glm::rotate(modelMatrix, 20 / (pow(layer.time.total, 4.0f) + 1), Vec3(0, 0, 1));
	renderer.uniforms().setMatrix("mMatrix", modelMatrix);
	renderer.renderObjectSystem.render("logo");

	if (layer.time.paused) {
		layer.time.reset();
		layer.time.unpause();
	}

	if (layer.time.total > 5) {
		layer.destroy();
		layer = Layer(renderer, createMainMenuLayer, renderMainMenuLayer, nullptr);
	}
}

void createMainMenuLayer(Layer & layer)
{
	layer.rendererPtr->renderObjectSystem.addRenderObject("mainMenu", "postProcessPlane", "MainMenuDesignPrototype", "unlit");
}
void renderMainMenuLayer(Layer & layer)
{
	layer.time.update();

	Renderer& renderer = *layer.rendererPtr;

	Float fade = 0.75f * (Float(layer.time.total) - 1.0f);

	renderer.viewportSystem.select("full");
	renderer.viewportSystem.render(renderer.framebufferSystem.current());

	renderer.clearColor(Color(0, 0, 0, 1));
	renderer.clearDepth();
	renderer.setCulling(true);
	renderer.setDepthTest(true);
	renderer.setAlphaBlending(true);
	renderer.setWireframeMode(false);

	//main menu stuff
	renderer.resetModelMatrix();
	renderer.uniforms().setMatrix("vpMatrix", Matrix(1));
	renderer.uniforms().setFloat("fade", fade);
	renderer.uniforms().setFloat("tint", 1);
	renderer.renderObjectSystem.render("mainMenu");

	if (layer.time.total > 3) {
		layer.destroy();
		layer = Layer(renderer, createGameInstanceLayer, renderGameInstanceLayer, nullptr);
	}

	if (layer.time.paused) {
		layer.time.reset();
		layer.time.unpause();
	}
}

void createGameInstanceLayer(Layer & layer)
{
	RenderObjectSystem& renderObjectSystem = layer.rendererPtr->renderObjectSystem;
	renderObjectSystem.addRenderObject("sky", "sky", "stars", "sky");
	renderObjectSystem.addRenderObject("plane", "plane", "default", "primitive");
	renderObjectSystem.addRenderObject("monkey", "monkey", "default", "primitive");
	renderObjectSystem.addRenderObject("default", "default", "default", "primitive");
	renderObjectSystem.addRenderObject("wireBox", "wireMeshBox", "default", "primitive");
	renderObjectSystem.addRenderObject("boundingBox", "boundingBox", "default", "primitive");
}
void renderGameInstanceLayer(Layer & layer)
{
	Renderer& renderer = *layer.rendererPtr;

	renderer.cameraSystem.currentPlanetCamera().update(renderer.renderTime);

	renderer.clearColor(Color(0, 0, 0, 0.75));

	//enables or disables wireframe mode
	renderer.setWireframeMode(renderer.wireframeMode);

	renderer.viewportSystem.select("full");
	renderer.viewportSystem.render(renderer.framebufferSystem.current());

	//render sky
	renderer.setCulling(false);
	renderer.setDepthTest(false);
	renderer.resetModelMatrix();
	renderer.uniforms().setVec3("chunkOffsetVector", Vec3(0));
	renderer.uniforms().setVec3("cameraVector", renderer.cameraSystem.currentPlanetCamera().forwardVector);
	renderer.uniforms().setMatrix("vpMatrix", renderer.cameraSystem.currentPlanetCamera().projectionMatrix(renderer.viewportSystem.current().aspectRatio()) * renderer.cameraSystem.currentPlanetCamera().viewMatrixOnlyRot());
	renderer.uniforms().setMatrix("mMatrix", Matrix(1));
	renderer.renderObjectSystem.render("sky");

	//render planet
	renderer.planetRenderSystem.render(gameSimulation.planetSystem, renderer);
}

void createAiPlaygroundLayer(Layer& layer) {
	RenderObjectSystem& renderObjectSystem = layer.rendererPtr->renderObjectSystem;
};
void renderAiPlaygroundLayer(Layer& layer) {
	Renderer& renderer = *layer.rendererPtr;


	renderer.clearColor(Color(0, 0, 0, 0.75));

	//enables or disables wireframe mode
	renderer.setWireframeMode(renderer.wireframeMode);

	renderer.viewportSystem.select("full");
	renderer.viewportSystem.render(renderer.currentFramebuffer());


};
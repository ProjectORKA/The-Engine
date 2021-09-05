
#include "UIGameView.hpp"
#include "Window.hpp"

//Map<Index, PlanetCamera> planetCameras;
//Map<Index, SimpleCamera> cameras;
//
//void createUIORKALogo(UIElement* element, Window& window) {
//	window.renderer.renderObjectSystem.addRenderObject("logo", "ProjectORKALogo", "ProjectORKABakedLogo", "unlit");
//	window.renderer.renderTime.paused = true;
//}
//void renderUIORKALogo(UIElement* element, Window& window, TiledRectangle screenArea) {
//	Renderer & renderer = window.renderer;
//
//	renderer.renderRegion.set(screenArea);
//
//	//renderer.time.update();
//	renderer.clearColor(Color(0));
//	renderer.clearDepth();
//
//	renderer.setCulling(true);
//	renderer.setDepthTest(true);
//	//renderer.setAlphaBlending(false);
//	renderer.setWireframeMode(false);
//
//	//camera stuff
//	SimpleCamera c;
//	c.location = Vec3(0, -15, 0);
//	renderer.uniforms().data.vpMatrix = c.projectionMatrix(renderer.aspectRatio()) * c.viewMatrix();
//
//	//logo stuff
//	Float animationLength = 1.1;
//	Float size = min(pow(1 + 1 / pow(animationLength, 5), pow(renderer.renderTime.total - 2 * animationLength, 5)), 100.0);
//	Float tint = max(2 - size, 0.0f);
//	renderer.uniforms().data.customColor = Color(tint, tint, tint, 1);
//	Matrix modelMatrix = glm::scale(Matrix(1), Vec3(size, 1, size));
//	modelMatrix = glm::rotate(modelMatrix, 20 / (pow(renderer.renderTime.total, 4.0f) + 1), Vec3(0, 0, 1));
//	renderer.uniforms().data.mMatrix = modelMatrix;
//	renderer.uniforms().update();
//
//	renderer.renderObjectSystem.render("logo");
//
//	if (renderer.renderTime.paused) { //actually starts the animation by unpausing the timer
//		renderer.renderTime.reset();
//		renderer.renderTime.unpause();
//	}
//
//	if (renderer.renderTime.total > 5) {
//		element->destroy(window);
//		*element = UIORKAGame(); //<--replaces a scene
//		element->create(window);
//	}
//}
//
//void createUIORKAGame(UIElement* element, Window& window) {
//	PlanetCamera& camera = planetCameras[element->id];
//	
//	camera.location = Vec3(LLONG_MAX, LLONG_MAX, 0);
//
//	element->createChildren(window);
//}
//void renderUIORKAGame(UIElement* element, Window& window, TiledRectangle screenArea) {
//
//	Renderer& renderer = window.renderer;				//get actual renderer
//	PlanetCamera& planetCamera = planetCameras[0];	//get camera
//	if (inputManager.capturing)planetCamera.rotate(Vec2(inputManager.cursorPosition) * inputManager.mouseSensitivity);
//
//	planetCamera.speedMultiplier = inputManager.scrollAxisYTotal;
//	if (inputManager.forward.isPressed)	planetCamera.accelerationVector += planetCamera.forwardVector;
//	if (inputManager.backward.isPressed) planetCamera.accelerationVector -= planetCamera.forwardVector;
//	if (inputManager.up.isPressed) planetCamera.accelerationVector += planetCamera.upVector;
//	if (inputManager.down.isPressed) planetCamera.accelerationVector -= planetCamera.upVector;
//	if (inputManager.right.isPressed) planetCamera.accelerationVector += planetCamera.rightVector;
//	if (inputManager.left.isPressed) planetCamera.accelerationVector -= planetCamera.rightVector;
//
//	planetCamera.update(renderer.renderTime.delta);
//
//	//enables or disables wireframe mode
//	renderer.setWireframeMode(renderer.wireframeMode);
//
//	//render sky
//	renderer.setCulling(false);
//	renderer.setDepthTest(false);
//
//	renderer.shaderSystem.uniforms.data.vpMatrix = planetCamera.projectionMatrix(renderer.renderRegion.getAspectRatio()) * planetCamera.viewMatrixOnlyRot();
//	renderer.shaderSystem.uniforms.update();
//
//	renderer.shaderSystem.use("sky");
//	renderer.textureSystem.use("sky");
//	renderer.meshSystem.render("sky");
//
//	//render planet
//	
//	renderer.planetRenderSystem.render(gameSimulation.planetSystem, renderer, planetCamera);
//	//renderer.clearDepth();
//
//	//renderer.shaderSystem.uniforms.data.chunkOffsetVector = Vec4((Vec3(planetCamera.chunkLocation) + planetCamera.location) / Vec3(ULLONG_MAX) ,1);
//	//renderer.shaderSystem.uniforms.update();
//	//renderer.sdfTerrainRenderSystem.render(renderer);
//
//	//for (UIElement* childElement : element->content) {
//	//	childElement->render(window, childArea);
//	//}
//}

//UIORKALogo::UIORKALogo() {
//	createFunction = createUIORKALogo;
//	updateFunction = updateUIElement;
//	renderFunction = renderUIORKALogo;
//	destroyFunction = destroyUIElement;
//}
//UIORKAGame::UIORKAGame() {
//	createFunction = createUIORKAGame;
//	updateFunction = updateUIElement;
//	renderFunction = renderUIORKAGame;
//	destroyFunction = destroyUIElement;
//}


void renderUIGameView(UIElement* element, Window& window, TiledRectangle screenArea){
	UIGameView* gameView = static_cast<UIGameView*>(element);
	gameView->game->render(window);
};

void updateUIGameView(UIElement* element, Window& window) {
	UIGameView * gameView = static_cast<UIGameView *>(element);
	gameView->game->update();
};
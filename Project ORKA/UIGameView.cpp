
#include "UIGameView.hpp"
#include "Window.hpp"

PlanetCamera camera;

void createUIORKALogo(UIElement* element, Window& window) {
	window.renderer.renderObjectSystem.addRenderObject("logo", "ProjectORKALogo", "ProjectORKABakedLogo", "unlit");
	window.renderer.renderTime.paused = true;
}
void renderUIORKALogo(UIElement* element, Window& window, TiledRectangle screenArea) {
	Renderer & renderer = window.renderer;

	renderer.renderRegion.setRenderRegion(screenArea);

	//renderer.time.update();
	renderer.clearColor(Color(0));
	renderer.clearDepth();

	renderer.setCulling(true);
	renderer.setDepthTest(true);
	//renderer.setAlphaBlending(false);
	renderer.setWireframeMode(false);

	//camera stuff
	SimpleCamera c;
	c.location = Vec3(0, -15, 0);
	renderer.uniforms().data.vpMatrix = c.projectionMatrix(renderer.aspectRatio()) * c.viewMatrix();

	//logo stuff
	Float animationLength = 1.1;
	Float size = min(pow(1 + 1 / pow(animationLength, 5), pow(renderer.renderTime.total - 2 * animationLength, 5)), 100.0);
	Float tint = max(2 - size, 0.0f);
	renderer.uniforms().data.customColor = Color(tint, tint, tint, 1);
	Matrix modelMatrix = glm::scale(Matrix(1), Vec3(size, 1, size));
	modelMatrix = glm::rotate(modelMatrix, 20 / (pow(renderer.renderTime.total, 4.0f) + 1), Vec3(0, 0, 1));
	renderer.uniforms().data.mMatrix = modelMatrix;
	renderer.uniforms().update();

	renderer.renderObjectSystem.render("logo");

	if (renderer.renderTime.paused) { //actually starts the animation by unpausing the timer
		renderer.renderTime.reset();
		renderer.renderTime.unpause();
	}

	if (renderer.renderTime.total > 5) {
		element->destroy(window);
		*element = UIORKAGame(); //<--replaces a scene
		element->create(window);
	}
}

void createUIORKAGame(UIElement* element, Window& window) {
		camera.location = Vec3(LLONG_MAX, LLONG_MAX, 0);

		RenderObjectSystem& ros = window.renderer.renderObjectSystem;

		ros.addRenderObject("sky", "sky", "sky", "unlit");
		ros.addRenderObject("plane", "plane", "default", "primitive");
		ros.addRenderObject("monkey", "monkey", "default", "primitive");
		ros.addRenderObject("default", "default", "default", "primitive");
		ros.addRenderObject("wireBox", "wireMeshBox", "default", "primitive");
		ros.addRenderObject("boundingBox", "boundingBox", "default", "primitive");

		element->createChildren(window);
}
void renderUIORKAGame(UIElement* element, Window& window, TiledRectangle screenArea) {

	Renderer& renderer = window.renderer;

	if (inputManager.capturing)camera.rotate(Vec2(inputManager.cursorPosition) * inputManager.mouseSensitivity);

	camera.speedMultiplier = inputManager.scrollAxisYTotal;


	if (inputManager.forward.isPressed)	camera.accelerationVector += camera.forwardVector;
	if (inputManager.backward.isPressed) camera.accelerationVector -= camera.forwardVector;
	if (inputManager.up.isPressed) camera.accelerationVector += camera.upVector;
	if (inputManager.down.isPressed) camera.accelerationVector -= camera.upVector;
	if (inputManager.right.isPressed) camera.accelerationVector += camera.rightVector;
	if (inputManager.left.isPressed) camera.accelerationVector -= camera.rightVector;
	camera.update(renderer.renderTime);


	renderer.clearColor(Color(0, 0, 0, 0.75));

	//enables or disables wireframe mode
	renderer.setWireframeMode(renderer.wireframeMode);

	//render sky
	renderer.setCulling(false);
	renderer.setDepthTest(false);
	renderer.shaderSystem.uniforms.data.mMatrix = Matrix(1);
	renderer.shaderSystem.uniforms.data.customColor = Vec4(1);
	renderer.shaderSystem.uniforms.data.chunkOffsetVector = Vec4(0);
	renderer.shaderSystem.uniforms.data.distortion = renderer.planetRenderSystem.quadtreeRenderSystem.worldDistortion;
	renderer.shaderSystem.uniforms.data.cameraVector = Vec4(renderer.cameraSystem.currentPlanetCamera().forwardVector, 0);
	renderer.shaderSystem.uniforms.data.vpMatrix = camera.projectionMatrix(renderer.renderRegion.getAspectRatio()) * camera.viewMatrixOnlyRot();
	renderer.shaderSystem.uniforms.update();
	renderer.renderObjectSystem.render("sky");

	//render planet
	renderer.planetRenderSystem.render(gameSimulation.planetSystem, renderer, camera);

	//render text
	renderer.setDepthTest(false);
	renderer.shaderSystem.uniforms.data.mMatrix = Matrix(1);
	renderer.shaderSystem.uniforms.data.customColor = Color(1);

	String s = std::to_string(camera.speedMultiplier);
	renderer.textRenderSystem.render(s, 0, 0, 1);


	//render other UI
	TiledRectangle childArea = screenArea;
	Int cutaWay = 50;
	childArea.position.y += childArea.position.y + childArea.size.y - cutaWay;
	childArea.size.y = cutaWay;

	for (UIElement* childElement : element->content) {
		childElement->render(window, childArea);
	}
}

void createUIORKA3DPlayground(UIElement* element, Window& window) {
	camera.location = Vec3(LLONG_MAX, LLONG_MAX, 0);

	RenderObjectSystem& ros = window.renderer.renderObjectSystem;

	//

	element->createChildren(window);
}
void renderUIORKA3DPlayground(UIElement* element, Window& window, TiledRectangle screenArea) {

	Renderer& renderer = window.renderer;

	if (inputManager.capturing)camera.rotate(Vec2(inputManager.cursorPosition) * inputManager.mouseSensitivity);

	camera.speedMultiplier = inputManager.scrollAxisYTotal;

	if (inputManager.forward.isPressed)	camera.accelerationVector += camera.forwardVector;
	if (inputManager.backward.isPressed) camera.accelerationVector -= camera.forwardVector;
	if (inputManager.up.isPressed) camera.accelerationVector += camera.upVector;
	if (inputManager.down.isPressed) camera.accelerationVector -= camera.upVector;
	if (inputManager.right.isPressed) camera.accelerationVector += camera.rightVector;
	if (inputManager.left.isPressed) camera.accelerationVector -= camera.rightVector;
	camera.update(renderer.renderTime);


	renderer.clearColor(Color(0, 0, 0, 0.75));

	//enables or disables wireframe mode
	renderer.setWireframeMode(renderer.wireframeMode);

	//render sky
	renderer.setCulling(false);
	renderer.setDepthTest(false);
	renderer.shaderSystem.uniforms.data.mMatrix = Matrix(1);
	renderer.shaderSystem.uniforms.data.customColor = Vec4(1);
	renderer.shaderSystem.uniforms.data.chunkOffsetVector = Vec4(0);
	renderer.shaderSystem.uniforms.data.distortion = renderer.planetRenderSystem.quadtreeRenderSystem.worldDistortion;
	renderer.shaderSystem.uniforms.data.cameraVector = Vec4(renderer.cameraSystem.currentPlanetCamera().forwardVector, 0);
	renderer.shaderSystem.uniforms.data.vpMatrix = camera.projectionMatrix(renderer.renderRegion.getAspectRatio()) * camera.viewMatrixOnlyRot();
	renderer.shaderSystem.uniforms.update();
	renderer.renderObjectSystem.render("sky");

	//render planet
	renderer.planetRenderSystem.render(gameSimulation.planetSystem, renderer, camera);

	//render text
	renderer.setDepthTest(false);
	renderer.shaderSystem.uniforms.data.mMatrix = Matrix(1);
	renderer.shaderSystem.uniforms.data.customColor = Color(1);

	//render other UI
	TiledRectangle childArea = screenArea;
	Int cutaWay = 50;
	childArea.position.y += childArea.position.y + childArea.size.y - cutaWay;
	childArea.size.y = cutaWay;

	element->renderChildren(window, screenArea);
}

UIORKALogo::UIORKALogo() {
	createFunction = createUIORKALogo;
	updateFunction = updateUIElement;
	renderFunction = renderUIORKALogo;
	destroyFunction = destroyUIElement;
}
UIORKAGame::UIORKAGame() {
	createFunction = createUIORKAGame;
	updateFunction = updateUIElement;
	renderFunction = renderUIORKAGame;
	destroyFunction = destroyUIElement;
}
UIORKA3DPlayground::UIORKA3DPlayground() {
	
}
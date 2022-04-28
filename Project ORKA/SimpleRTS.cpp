
#include "SimpleRTS.hpp"
#include "Scene.hpp"

void SimpleRTSRenderer::update(Renderer& renderer) {
	player.speed = pow(1.2, player.speedExponent);

	if (forward.pressed)	player.accelerationVector += player.camera.forwardVector;
	if (backward.pressed)	player.accelerationVector -= player.camera.forwardVector;
	if (upward.pressed)		player.accelerationVector += player.camera.upVector;
	if (downward.pressed)	player.accelerationVector -= player.camera.upVector;
	if (right.pressed)		player.accelerationVector += player.camera.rightVector;
	if (left.pressed)		player.accelerationVector -= player.camera.rightVector;

	player.update(renderer);
}
SimpleRTSRenderer::SimpleRTSRenderer(SimpleRTSSimulation& sim) {
	this->sim = &sim;
}
void SimpleRTSRenderer::mouseIsMoving(Window& window, IVec2 position) {
	if (inputManager.isCapturing(window))player.camera.rotate(Vec2(position) * Vec2(mouseSensitivity));
}
void SimpleRTSRenderer::render(TiledRectangle area, Renderer& renderer) {
	mutex.lock();

	renderer.setWireframeMode();
	renderer.setCulling(true);
	renderer.clearDepth();

	//sky
	renderer.setDepthTest(false);
	renderer.useShader("color");
	renderer.fill(Color(0.207143, 0.722031, 1.0f, 1));
	renderer.renderMesh("fullScreenQuad");
	renderer.setDepthTest(true);

	//prepare rendering scene
	renderer.uniforms().mMatrix() = Matrix(1);
	renderer.useShader("simpleRTS");
	player.render(renderer);

	//ground plane
	renderer.fill(Color(0.287249, 0.8, 0, 1));
	renderer.uniforms().mMatrix(matrixFromScale(SIMPLERTS_MAPSIZE));
	renderer.renderMesh("fullScreenQuad");

	//render trees that are about to be cut
	renderer.fill(SIMPLERTS_TREE_COLOR);
	for (UInt i = 0; i < sim->humanCount; i++) {
		if (sim->humanState[i] == SimpleRTSSimulation::SimpleRTSHumanState::LookingForWood) {
			renderer.uniforms().mMatrix(matrixFromLocation(sim->humanTargetpositions[i]));
			renderer.renderMesh("Tree");
		}
	}

	//prepare for instancing
	renderer.uniforms().mMatrix(Matrix(1));

	//trees
	renderer.fill(SIMPLERTS_TREE_COLOR);
	renderer.matrixSystem.matrixArray(sim->treePosition, sim->treeDirection, sim->treeSize);
	renderer.renderMeshInstanced("Tree");

	//render tree stumps
	renderer.fill(SIMPLERTS_BARK_COLOR);
	renderer.matrixSystem.matrixArray(sim->treeStumpPosition);
	renderer.renderMeshInstanced("TreeStump");

	renderer.fill(Color(1, 0.538269, 0.334333, 1));
	renderer.matrixSystem.matrixArray(sim->humanPositions, sim->humanDirection);
	renderer.renderMeshInstanced("Human");

	//render bases
	renderer.fill(Color(0.085714f, 0.039626f, 0.015222f, 1.0f));
	renderer.matrixSystem.matrixArray(sim->basePosition);
	renderer.renderMeshInstanced("Base");

	//render rabbits
	renderer.fill(SIMPLERTS_RABBIT_COLOR);
	renderer.matrixSystem.matrixArray(sim->rabbitPosition, sim->rabbitVelocity);
	renderer.renderMeshInstanced("Rabbit");

	//ui
	renderer.setDepthTest(false);
	renderer.screenSpace();
	renderer.renderText(String(toString(1.0f / renderer.time.delta)), Vec2(50), fonts.heading);

	////////////////////////

	mutex.unlock();
}
void SimpleRTSRenderer::mouseIsScrolled(Window& window, Double xAxis, Double yAxis) {
	player.speedExponent += yAxis;
}
void SimpleRTSRenderer::buttonIsPressed(Window& window, Key key, ActionState action, Int modifiers) {
	Bool pressed = (action != ActionState::Release);
	switch (key) {
	case Key::F: if (pressed) window.renderer.wireframeMode = !window.renderer.wireframeMode;
		break;
	case Key::W: forward.pressed = pressed;
		break;
	case Key::S: backward.pressed = pressed;
		break;
	case Key::A: left.pressed = pressed;
		break;
	case Key::D: right.pressed = pressed;
		break;
	case Key::Q: downward.pressed = pressed;
		break;
	case Key::E: upward.pressed = pressed;
		break;
	default:
		break;
	}
}
void SimpleRTSRenderer::mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers) {
	if (button == MouseButton::LEFT && action == ActionState::Press) inputManager.captureCursor(window);
	if (button == MouseButton::RIGHT && action == ActionState::Press) inputManager.uncaptureCursor(window);
}
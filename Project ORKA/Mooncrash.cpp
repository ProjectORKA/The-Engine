#include "Mooncrash.hpp"
#include "Window.hpp"

void renderUI(Renderer& renderer, MooncrashPlayer& player) {
	renderer.setWireframeMode(false);

	String s = String("FrameTime: ").append(toString(renderer.renderTime.delta));
	renderer.textRenderSystem.render(s, 0, 0, fonts.debug);

	s = String("FPS: ").append(toString(1.0f / renderer.renderTime.delta));
	renderer.textRenderSystem.render(s, 0, fonts.debug.absoluteSize, fonts.debug);

	s = String("Speed: ").append(toString(inputManager.scrollAxisYTotal));
	renderer.textRenderSystem.render(s, 0, 2 * fonts.debug.absoluteSize, fonts.debug);

	s = String("Camera height float: ").append(toString(player.camera.location.z));
	renderer.textRenderSystem.render(s, 0, 3 * fonts.debug.absoluteSize, fonts.debug);

	s = String("Camera height ULL: ").append(toString(player.chunkLocation.z));
	renderer.textRenderSystem.render(s, 0, 4 * fonts.debug.absoluteSize, fonts.debug);
}
void renderSky(Renderer& renderer, Camera & camera) {
	renderer.setCulling(false);
	renderer.setDepthTest(false);

	camera.renderOnlyRot(renderer);

	renderer.useShader("sky");
	renderer.useTexture("stars2");
	renderer.renderMesh("sky");
}
void renderPlanet(Renderer& renderer, PlanetSystem& planetSystem, PlanetSystemPlayer & player) {
	renderer.planetRenderSystem.render(planetSystem, renderer, player);
};

void MooncrashPlayer::update(Float delta) {
	speed = pow(CAMERA_SPEED_MULTIPLIER, speedMultiplier);
	accelerationVector *= speed * delta;

	camera.location += accelerationVector;

	//apply sub chunk location
	if (floor(camera.location.x) != 0) {
		if (floor(camera.location.x) > 0) {
			ULL chunkDelta = floor(camera.location.x);
			camera.location.x -= chunkDelta;
			chunkLocation.x += chunkDelta;
		}
		else {
			ULL chunkDelta = -floor(camera.location.x);
			camera.location.x += chunkDelta;
			chunkLocation.x -= chunkDelta;
		}
	}
	if (floor(camera.location.y) != 0) {
		if (floor(camera.location.y) > 0) {
			ULL chunkDelta = floor(camera.location.y);
			camera.location.y -= chunkDelta;
			chunkLocation.y += chunkDelta;
		}
		else {
			ULL chunkDelta = -floor(camera.location.y);
			camera.location.y += chunkDelta;
			chunkLocation.y -= chunkDelta;
		}
	}
	if (floor(camera.location.z) != 0) {
		if (floor(camera.location.z) > 0) {
			ULL chunkDelta = floor(camera.location.z);
			if (chunkLocation.z + chunkDelta > chunkLocation.z) {
				camera.location.z -= chunkDelta;
				chunkLocation.z += chunkDelta;
			}
			else {
				//camera is outside world bounds, but we will render anyway
				chunkDelta = ULLONG_MAX - chunkLocation.z;
				chunkLocation.z = ULLONG_MAX;
				camera.location.z -= chunkDelta;
			}
		}
		else {
			ULL chunkDelta = -floor(camera.location.z);
			if (chunkDelta > chunkLocation.z) {
				chunkLocation.z = 0;
				camera.location.z = 0;
			}
			else {
				camera.location.z += chunkDelta;
				chunkLocation.z -= chunkDelta;
			}
		}
	}

	accelerationVector = { 0,0,0 };

	if (CAMERA_TERRAIN_LIMIT) {
		LDouble oldCamHeight = LDouble(chunkLocation.z) + LDouble(camera.location.z);

		LDouble xLocation = camera.location.x;
		LDouble yLocation = camera.location.y;

		xLocation += LDouble(chunkLocation.x) / pow(2, 64);
		yLocation += LDouble(chunkLocation.y) / pow(2, 64);

		LDouble newCamHeight = pow(2, 64 - 20) + terrainGenerationFunction(xLocation, yLocation);

		if (newCamHeight > oldCamHeight) {
			ULL chunkheight = newCamHeight;
			newCamHeight -= chunkheight;
			chunkLocation.z = chunkheight;
			camera.location.z = newCamHeight;
		}
	}
}

void MooncrashAtmosphere::render(MooncrashPlayer player, Renderer& renderer)
{
	Bool culling = renderer.getCulling();
	player.camera.renderOnlyRot(renderer);
	renderer.uniforms().data.cameraPosition = Vec4(Vec3(0,0,(Float(player.chunkLocation.z) + player.camera.location.z)/Float(ULLONG_MAX)), 1);
	renderer.setCulling(false);
	renderer.uniforms().data.sunDir = Vec4(normalize(Vec3(0, 1, 1 + sin(renderer.renderTime.total / 2))), 1);
	renderer.uniforms().update();
	renderer.useShader("atmosphere");

	renderer.framebufferSystem.current().colorTexture.use(0);
	renderer.framebufferSystem.current().depthTexture.use(1);

	//renderer.setAlphaBlending(true);
	//glBlendFunc(GL_ONE, GL_ONE);
	renderer.setDepthTest(false);

	renderer.renderMesh("centeredCube");
	renderer.setCulling(culling);

	//renderer.setAlphaBlending(false);
	renderer.setDepthTest(true);
}

void Mooncrash::update() {
	planetSystem.update();
}
void Mooncrash::render(Renderer& renderer) {

	player.speedMultiplier = inputManager.scrollAxisYTotal;
	if (forward.pressed)	player.accelerationVector += player.camera.forwardVector;
	if (backward.pressed)	player.accelerationVector -= player.camera.forwardVector;
	if (upward.pressed)		player.accelerationVector += player.camera.upVector;
	if (downward.pressed)	player.accelerationVector -= player.camera.upVector;
	if (right.pressed)		player.accelerationVector += player.camera.rightVector;
	if (left.pressed)		player.accelerationVector -= player.camera.rightVector;

	player.update(renderer.renderTime.delta);

	renderer.setWireframeMode(renderer.wireframeMode);

	renderSky(renderer, player.camera);


	renderPlanet(renderer, planetSystem, player);

	atmos.render(player, renderer);

	//renderUI(renderer, player);
}
void Mooncrash::mouseIsMoving(Window& window, IVec2 position) {
	if (inputManager.isCapturing(window))player.camera.rotate(Vec2(position) * Vec2(mouseSensitivity));
}
void Mooncrash::buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) {
	if (action == GLFW_PRESS) {
		switch (keyID) {
		case GLFW_KEY_F: window.renderer.wireframeMode = !window.renderer.wireframeMode;
			break;
		case GLFW_KEY_J: window.renderer.adjustRenderVariables = !window.renderer.adjustRenderVariables;
			break;
		//case GLFW_KEY_K: window.renderer.planetRenderSystem.worldDistortion = !window.renderer.planetRenderSystem.worldDistortion;
		//	break;
		//case GLFW_KEY_G: window.renderer.planetRenderSystem.chunkBorders = !window.renderer.planetRenderSystem.chunkBorders;
		//	break;
		case GLFW_KEY_T:
			window.renderer.mutex.lock();
			window.renderer.shaderSystem.rebuild();
			window.renderer.mutex.unlock();
			break;
		case GLFW_KEY_W: forward.pressed = true;
			break;
		case GLFW_KEY_S: backward.pressed = true;
			break;
		case GLFW_KEY_A: left.pressed = true;
			break;
		case GLFW_KEY_D: right.pressed = true;
			break;
		case GLFW_KEY_Q: downward.pressed = true;
			break;
		case GLFW_KEY_E: upward.pressed = true;
			break;
		default:
			break;
		}
	}

	if (action == GLFW_RELEASE) {
		switch (keyID) {
		case GLFW_KEY_W: forward.pressed = false;
			break;
		case GLFW_KEY_S: backward.pressed = false;
			break;
		case GLFW_KEY_A: left.pressed = false;
			break;
		case GLFW_KEY_D: right.pressed = false;
			break;
		case GLFW_KEY_Q: downward.pressed = false;
			break;
		case GLFW_KEY_E: upward.pressed = false;
			break;
		default:
			break;
		}
	}
}
void Mooncrash::mouseIsPressed(Window& window, Int button, Int action, Int modifiers) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) inputManager.captureCursor(window);

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) inputManager.uncaptureCursor(window);
}
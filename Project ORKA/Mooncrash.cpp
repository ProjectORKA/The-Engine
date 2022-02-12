#include "Mooncrash.hpp"
#include "Window.hpp"

MooncrashRenderer::MooncrashRenderer(MooncrashSimulation * simulation) {
	player.speedExponent = 200;
	this->simulation = simulation;
}
void MooncrashRenderer::render(TiledRectangle area, Renderer& renderer) {
	if (forward.pressed)	player.accelerationVector += player.camera.forwardVector;
	if (backward.pressed)	player.accelerationVector -= player.camera.forwardVector;
	if (upward.pressed)		player.accelerationVector += player.camera.upVector;
	if (downward.pressed)	player.accelerationVector -= player.camera.upVector;
	if (right.pressed)		player.accelerationVector += player.camera.rightVector;
	if (left.pressed)		player.accelerationVector -= player.camera.rightVector;

	player.render(renderer, skyRotationMatrix);

	renderer.uniforms().sunDir(skyRotationMatrix * Vec4(0,0,1,0));

	renderer.setWireframeMode(renderer.wireframeMode);

	renderer.uniforms().mMatrix(skyRotationMatrix);
	renderer.renderSky(player.camera);

	//renderMooncrashAtmosphere(renderer, player);

	renderPlanet(renderer, simulation->planetSystem, player);
	//renderUI(renderer, player);
}
void renderUI(Renderer& renderer, MooncrashPlayer& player) {
	renderer.setWireframeMode(false);
	renderer.setDepthTest(false);

	renderer.screenSpace();

	String s = String("FrameTime: ").append(toString(renderer.time.delta));
	renderer.renderText(s, Vec2(0), fonts.debug);

	s = String("FPS: ").append(toString(1.0f / renderer.time.delta));
	renderer.renderText(s, Vec2(0, fonts.debug.absoluteSize), fonts.debug);

	s = String("Speed: ").append(toString(player.speedExponent));
	renderer.renderText(s, Vec2(0, 2 * fonts.debug.absoluteSize), fonts.debug);

	s = String("Camera height float: ").append(toString(player.camera.location.z));
	renderer.renderText(s, Vec2(0, 3 * fonts.debug.absoluteSize), fonts.debug);

	s = String("Camera height ULL: ").append(toString(player.chunkLocation.z));
	renderer.renderText(s, Vec2(0, 4 * fonts.debug.absoluteSize), fonts.debug);

	s = String("SunDir: ").append(toString(renderer.uniforms().sunDir().x).append(toString(renderer.uniforms().sunDir().y)).append(toString(renderer.uniforms().sunDir().z)));
	renderer.renderText(s, Vec2(0, 5 * fonts.debug.absoluteSize), fonts.debug);
}
void MooncrashRenderer::mouseIsMoving(Window& window, IVec2 position) {
	if (inputManager.isCapturing(window))player.camera.rotate(Vec2(position) * Vec2(mouseSensitivity));
}
void MooncrashPlayer::render(Renderer& renderer, Matrix& skyRotation) {
	speed = pow(1.2f, speedExponent);
	accelerationVector *= speed * renderer.time.delta;

	Float planetRadius = 4;
	Vec3 v1 = rotate(Vec3(skyRotation[0]), -accelerationVector.x * Float(planetRadius) / Float(ULLONG_MAX), Vec3(0, 1, 0));
	Vec3 v2 = rotate(Vec3(skyRotation[0]), +accelerationVector.y * Float(planetRadius) / Float(ULLONG_MAX), Vec3(1, 0, 0));
	Vec3 v3 = rotate(Vec3(skyRotation[1]), -accelerationVector.x * Float(planetRadius) / Float(ULLONG_MAX), Vec3(0, 1, 0));
	Vec3 v4 = rotate(Vec3(skyRotation[1]), +accelerationVector.y * Float(planetRadius) / Float(ULLONG_MAX), Vec3(1, 0, 0));
	skyRotation[0] = Vec4(normalize(v1 + v2), 0);
	skyRotation[1] = Vec4(normalize(v3 + v4), 0);
	skyRotation[2] = Vec4(normalize(cross(Vec3(skyRotation[0]), Vec3(skyRotation[1]))), 0);
	skyRotation[3] = Vec4(0, 0, 0, 1);

	//logDebug(skyRotation);

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

	//if (CAMERA_TERRAIN_LIMIT) {
	//LDouble oldCamHeight = LDouble(chunkLocation.z) + LDouble(camera.location.z);

	//LDouble xLocation = camera.location.x;
	//LDouble yLocation = camera.location.y;

	//xLocation += LDouble(chunkLocation.x) / pow(2, 64);
	//yLocation += LDouble(chunkLocation.y) / pow(2, 64);

	//LDouble newCamHeight = pow(2, 64 - 20) + terrainGenerationFunction(xLocation, yLocation);

	//if (newCamHeight > oldCamHeight) {
	//		ULL chunkheight = newCamHeight;
	//		newCamHeight -= chunkheight;
	//		chunkLocation.z = chunkheight;
	//		camera.location.z = newCamHeight;
	//	}
	//}

	camera.render(renderer);
}
void renderMooncrashAtmosphere(Renderer& renderer, MooncrashPlayer& player)
{
	Bool culling = renderer.getCulling();
	player.camera.renderOnlyRot(renderer);
	renderer.uniforms().cameraPos() = Vec4(Vec3(0, 0, (Float(player.chunkLocation.z) + player.camera.location.z) / Float(ULLONG_MAX)), 1);
	renderer.setCulling(false);
	renderer.useShader("atmosphere");
	renderer.framebufferSystem.current().setAsTexture(0);

	//renderer.setAlphaBlending(true);
	//glBlendFunc(GL_ONE, GL_ONE);
	renderer.setDepthTest(false);

	renderer.renderMesh("centeredCube");
	renderer.setCulling(culling);

	//renderer.setAlphaBlending(false);
	renderer.setDepthTest(true);
}
void MooncrashRenderer::mouseIsScrolled(Window& window, Double xAxis, Double yAxis) {
	player.speedExponent += yAxis;
}
void MooncrashRenderer::buttonIsPressed(Window& window, Key key, Int action, Int modifiers) {
	if (action == GLFW_PRESS) {
		
		Bool pressed = action == GLFW_PRESS;
		
		switch (key) {
		case Key::F: if(pressed) window.renderer.wireframeMode = !window.renderer.wireframeMode;
			break;
		case Key::J: if(pressed) window.renderer.adjustRenderVariables = !window.renderer.adjustRenderVariables;
			break;
		case Key::K: if(pressed) window.renderer.planetRenderSystem.worldDistortion = !window.renderer.planetRenderSystem.worldDistortion;
			break;
		case Key::T:
			if (pressed) {
				window.renderer.mutex.lock();
				window.renderer.shaderSystem.rebuild();
				window.renderer.mutex.unlock();
			}
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
		case Key::U: if(pressed) window.renderer.planetRenderSystem.quadtreeRenderSystem.count();
			break;
		default:
			break;
		}
	}
}
void MooncrashRenderer::mouseIsPressed(Window& window, MouseButton button, Int action, Int modifiers) {
	
	if (action == GLFW_PRESS) {
		if (button == MouseButton::L) inputManager.captureCursor(window);
		if (button == MouseButton::R) inputManager.uncaptureCursor(window);
	}
}
void renderPlanet(Renderer& renderer, PlanetSystem& planetSystem, PlanetSystemPlayer& player){
	renderer.setDepthTest(true);
	renderer.setAlphaBlending(false);
	renderer.planetRenderSystem.render(planetSystem, renderer, player);
};

void MooncrashSimulation::update() {
	planetSystem.update();
}

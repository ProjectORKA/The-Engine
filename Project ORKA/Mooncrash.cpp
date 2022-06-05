#include "Mooncrash.hpp"
#include "Window.hpp"

void MooncrashSimulation::update(Float timeStep) {
	planetSystem.update();
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
void renderMooncrashAtmosphere(Renderer& renderer, MooncrashPlayer& player)
{
	Bool culling = renderer.getCulling();
	player.camera.renderOnlyRot(renderer);
	renderer.uniforms().cameraPos() = Vec4(Vec3(0, 0, (Float(player.chunkLocation.z) + player.camera.location.z) / Float(ULLONG_MAX)), 1);
	renderer.setCulling(false);
	renderer.useShader("atmosphere");
	renderer.framebufferSystem.currentDraw().setAsTexture(0); //[TODO]might not work if draw doesent bind; check

	//renderer.setAlphaBlending(true);
	//glBlendFunc(GL_ONE, GL_ONE);
	renderer.setDepthTest(false);

	renderer.renderMesh("centeredCube");
	renderer.setCulling(culling);

	//renderer.setAlphaBlending(false);
	renderer.setDepthTest(true);
}
void renderPlanet(Renderer& renderer, PlanetSystem& planetSystem, PlanetSystemPlayer& player) {
	renderer.setDepthTest(true);
	renderer.setAlphaBlending(false);
	renderer.planetRenderSystem.render(planetSystem, renderer, player);
};

MooncrashPlayer::MooncrashPlayer() {
	chunkLocation.z = ULLONG_MAX / 2;
}
void MooncrashPlayer::update(Window& window) {
	//get frequently used info
	Float delta = window.renderer.deltaTime();

	//set up temporary data
	Vec3 movementVector = Vec3(0);
	Float desiredSpeed = 0;

	//process input
	if (window.capturing) camera.rotate(window.mouseDelta * MouseMovement(mouseSensitivity));
	if (window.pressed(forward)) movementVector += camera.forwardVector;
	if (window.pressed(backward)) movementVector -= camera.forwardVector;
	if (window.pressed(right)) movementVector += camera.rightVector;
	if (window.pressed(left)) movementVector -= camera.rightVector;
	if (window.pressed(up)) movementVector += camera.upVector;
	if (window.pressed(down)) movementVector -= camera.upVector;

	//calculate movement
	if (length(movementVector) > 0) {					//if there is movement input
		desiredSpeed = pow(baseNumber, speedExponent);	//calculate speed
		movementVector = normalize(movementVector);		//get direction of movement (just direction)
		movementVector *= desiredSpeed * delta;			//add speed to direction
		camera.location += movementVector;				//add it to cameras location
	}

	Float planetRadius = 4;
	Vec3 v1 = rotate(Vec3(skyRotation[0]), -movementVector.x * Float(planetRadius) / Float(ULLONG_MAX), Vec3(0, 1, 0));
	Vec3 v2 = rotate(Vec3(skyRotation[0]), +movementVector.y * Float(planetRadius) / Float(ULLONG_MAX), Vec3(1, 0, 0));
	Vec3 v3 = rotate(Vec3(skyRotation[1]), -movementVector.x * Float(planetRadius) / Float(ULLONG_MAX), Vec3(0, 1, 0));
	Vec3 v4 = rotate(Vec3(skyRotation[1]), +movementVector.y * Float(planetRadius) / Float(ULLONG_MAX), Vec3(1, 0, 0));
	skyRotation[0] = Vec4(normalize(v1 + v2), 0);
	skyRotation[1] = Vec4(normalize(v3 + v4), 0);
	skyRotation[2] = Vec4(normalize(cross(Vec3(skyRotation[0]), Vec3(skyRotation[1]))), 0);
	skyRotation[3] = Vec4(0, 0, 0, 1);

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
}
void MooncrashPlayer::render(Window & window) {
	Renderer& renderer = window.renderer;

	camera.render(renderer);
}

void MooncrashRenderer::update(Window& window) {
	player.update(window);
}
void MooncrashRenderer::render(Window& window, TiledRectangle area) {
	Renderer& renderer = window.renderer;

	player.render(window);

	renderer.uniforms().sunDir(player.skyRotation * Vec4(0, 0, 1, 0));

	renderer.setWireframeMode(renderer.wireframeMode);

	renderer.uniforms().mMatrix(player.skyRotation);
	renderer.renderSky(player.camera);

	//renderMooncrashAtmosphere(renderer, player);

	renderPlanet(renderer, simulation->planetSystem, player);
	//renderUI(renderer, player);
}
void MooncrashRenderer::inputEvent(Window& window, InputEvent input) {
	if (input == wireframeToggle) window.renderer.wireframeMode = !window.renderer.wireframeMode;
	if (input == countNodesButton) window.renderer.planetRenderSystem.quadtreeRenderSystem.count();
	if (input == worldDistortion) window.renderer.planetRenderSystem.worldDistortion = !window.renderer.planetRenderSystem.worldDistortion;
	if (input == rebuildShaders) {
		window.renderer.mutex.lock();
		window.renderer.shaderSystem.rebuild();
		window.renderer.mutex.unlock();
	}
	if (input == enter) window.captureCursor();
	if (input == exit) window.uncaptureCursor();

	player.inputEvent(window, input);
}
MooncrashRenderer::MooncrashRenderer(MooncrashSimulation& simulation) {
	player.speedExponent = 200;
	this->simulation = &simulation;
}
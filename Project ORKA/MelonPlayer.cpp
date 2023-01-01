
#include "MelonPlayer.hpp"
#include "Window.hpp"

MelonPlayer::MelonPlayer() {
	camera.fieldOfView = 90;
	camera.nearClipValue = 0.125;
	camera.farClipValue = 256;
}
void MelonPlayer::update(Window & window) {

	//mouse input
	if (window.capturing) {
		Vec3 delta = Vec3(Vec2(window.mouseDelta) * Vec2(mouseSensitivity), 0);
		delta *= (camera.location.z);
		velocity = Vec3(0);
		targetLocation += delta;
	}
	else {
		targetLocation = Vec3(window.mousePosBotLeft, 0);
	}

	//get frequently used info
	Float delta = window.renderer.deltaTime();

	//update target location
	targetLocation += velocity * delta;
	
	approach(location, targetLocation, delta * 300);

	//player location (smoothed)
	deltaLocation = location - lastLocation;
	lastLocationAtDistance = location + 0.01f * normalize(lastLocationAtDistance - location);
	Vec3 deltaToLastLocationAtDistance = location - lastLocationAtDistance;
	distanceTraveled = length(deltaLocation);
	distanceInRadians += distanceTraveled;
	distanceInRadians = mod(distanceInRadians, PI);
	if (distanceTraveled != 0) direction = normalize(deltaToLastLocationAtDistance);

	//update smoke
	smoke.update(location, window.renderer);

	//player rotation
	approach(currentVisibleDirection, direction, delta * 8);
	orientation = Orientation(currentVisibleDirection);
	lastLocation = location;

	//update camera
	Vec3 cameraTargetLocation = location + Vec3(0, 0, 1 + pow(1.2, zoomFactor));
	Vec3 cameraDesiredDelta = cameraTargetLocation - camera.location;
	camera.location += cameraDesiredDelta * delta * 10.0f;
}
void MelonPlayer::render(Engine& engine, Window & window) {
	Renderer& renderer = window.renderer;
	
	camera.setRotation(DVec3(PI/2,0,0));

	camera.render(renderer);

	//render player
	renderer.useShader(engine, "MelonUberShader");
	renderer.uniforms().mMatrix(matrixFromOrientation(orientation, location, 0.5) * rotate(Matrix(1), -2 * distanceInRadians, Vec3(1, 0, 0)));
	renderer.uniforms().customColor(Vec4(1));
	renderer.renderMesh(engine, "melonPlayerRolling");

	smoke.render(engine, renderer);
}
void MelonPlayer::inputEvent(Window& window, InputEvent input) {
	if (input == zoomIn)zoomFactor++;
	if (input == zoomOut)zoomFactor--;
	zoomFactor = clamp(zoomFactor, -3, 30);
	logDebug(zoomFactor);
}
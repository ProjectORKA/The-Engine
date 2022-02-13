
#include "MelonPlayer.hpp"
#include "Renderer.hpp"

MelonPlayer::MelonPlayer() {
	//set physics
	//mass = 1;
	//isDynamic = true;

	//set camera rotation
	camera.rotationX = -PI / 2;
	camera.fieldOfView = 90;
	camera.nearClipValue = 0.125;
	camera.farClipValue = 256;
	camera.update();
}
void MelonPlayer::update(Renderer& renderer) {
	//update target location
	targetLocation += velocity * renderer.time.delta;
	
	location = lerp(location,targetLocation,renderer.time.delta * 500);

	//player location (smoothed)
	deltaLocation = location - lastLocation;
	lastLocationAtDistance = location + 0.01f * normalize(lastLocationAtDistance - location);
	Vec3 deltaToLastLocationAtDistance = location - lastLocationAtDistance;
	distanceTraveled = length(deltaLocation);
	distanceInRadians += distanceTraveled;
	distanceInRadians = mod(distanceInRadians, PI);
	if (distanceTraveled != 0) direction = normalize(deltaToLastLocationAtDistance);

	//update smoke
	smoke.update(location, renderer);

	//player rotation
	approach(currentVisibleDirection, direction, renderer.time.delta * 8);
	orientation = Orientation(currentVisibleDirection);
	lastLocation = location;

	//update camera
	Vec3 cameraTargetLocation = location + Vec3(0, 0, 1 + pow(1.2, zoomFactor));
	Vec3 cameraDesiredDelta = cameraTargetLocation - camera.location;
	currentCameraVelocity = lerp(currentCameraVelocity, cameraDesiredDelta, renderer.time.delta * 10);
	currentCameraVelocity /= 1 + renderer.time.delta * 500;
	camera.location += currentCameraVelocity;
	
}
void MelonPlayer::render(Renderer& renderer) {
	camera.render(renderer);
	renderer.arrow(lastLocationAtDistance, location);

	//render player
	renderer.useShader("MelonUberShader");
	renderer.uniforms().mMatrix(matrixFromOrientation(orientation, location, 0.5) * rotate(Matrix(1), -2 * distanceInRadians, Vec3(1, 0, 0)));
	renderer.uniforms().customColor(Vec4(1));
	renderer.renderMesh("playerRolling");

	smoke.render(renderer);
}
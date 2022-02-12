
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
	//update location
	location += velocity * Vec3(2.0 / 144.0);


	deltaLocation = location - lastLocation;
	lastLocationAtDistance = location + normalize(lastLocationAtDistance - location) * Vec3(1);
	Vec3 deltaToLastLocationAtDistance = location - lastLocationAtDistance;
	distanceTraveled = length(deltaLocation);
	distanceInRadians += distanceTraveled;
	distanceInRadians = mod(distanceInRadians, PI);
	if (distanceTraveled != 0) direction = normalize(deltaLocation);
	for (Float i = 0; i < distanceTraveled * 32; i++) {
		smoke.spawn();
	}
	smoke.update(renderer);
	lastLocation = location;
}
void MelonPlayer::render(Renderer& renderer) {
	//render camera
	Vec3 cameraTargetLocation = location + Vec3(0, 0, 1 + pow(1.2, zoomFactor));
	Vec3 cameraDesiredDelta = cameraTargetLocation - camera.location;
	currentCameraVelocity = lerp(currentCameraVelocity, cameraDesiredDelta, renderer.time.delta * 2);
	currentCameraVelocity /= 1.2;
	camera.location += currentCameraVelocity;
	camera.render(renderer);

	//player rotation
	approach(currentVisibleDirection, direction, renderer.time.delta * 8);
	//currentVisibleDirection = normalize(currentVisibleDirection);
	Orientation o(currentVisibleDirection);

	//render scene
	renderer.uniforms().sunDir(Vec4(normalize(Vec3(-0.666, 0.333, 1)), 1));
	//render player

	//renderer.arrow(location, Vec3(0));

	renderer.useShader("MelonUberShader");
	renderer.uniforms().mMatrix(matrixFromOrientation(o, location, 0.5) * rotate(Matrix(1), -2 * distanceInRadians, Vec3(1, 0, 0)));
	renderer.uniforms().customColor(Vec4(1));
	renderer.renderMesh("playerRolling");

	smoke.render(renderer);
}
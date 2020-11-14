
#include "Camera.hpp"
#include "Renderer.hpp"

struct Viewport;

//camera system
void CameraSystem::create()
{
	add();
	select(0);
	current().rotate(0, 0);
}
void CameraSystem::add()
{
	cameras.emplace_back();
}
void CameraSystem::select(Index cameraID)
{
	currentCamera = cameraID;
}
void CameraSystem::render(Renderer& renderer)
{
	current().render(renderer);
}
void CameraSystem::destroy()
{
	cameras.clear();
}

Camera& CameraSystem::current()
{
	return cameras[currentCamera];
}

//camera
Matrix Camera::projectionMatrix(float aspectRatio)
{
	return glm::perspective(
		glm::radians(fieldOfView),
		aspectRatio,
		nearClipValue,
		farClipValue
	);
}
Matrix Camera::viewMatrix() {
	return glm::lookAt(
		location,
		location + forwardVector,
		upVector
	);
}
Matrix Camera::viewMatrixOnlyRot() {
	return glm::lookAt(
		Vec3(0),
		forwardVector,
		upVector
	);
}
void Camera::update(Time& renderTime)
{
	processLocation(renderTime);
}
void Camera::render(Renderer& renderer) {
	Float cameraHeight = (long double(chunkLocation.z) + location.z);
	renderer.uniforms().setFloat("cameraHeight", cameraHeight);
	renderer.uniforms().setVec3("cameraVector", forwardVector);
	renderer.uniforms().setMatrix("vpMatrix", projectionMatrix(renderer.currentViewport().aspectRatio()) * viewMatrixOnlyRot());
}
void Camera::applySubChunkLocation() {

	//all values > 1 or < 0 will be applied to the chunkLocation

	if (floor(location.x) != 0) {
		if (floor(location.x) > 0) {
			unsigned long long chunkDelta = floor(location.x);
			location.x -= chunkDelta;
			chunkLocation.x += chunkDelta;
		}
		else {
			unsigned long long chunkDelta = -floor(location.x);
			location.x += chunkDelta;
			chunkLocation.x -= chunkDelta;
		}
	}

	if (floor(location.y) != 0) {
		if (floor(location.y) > 0) {
			unsigned long long chunkDelta = floor(location.y);
			location.y -= chunkDelta;
			chunkLocation.y += chunkDelta;
		}
		else {
			unsigned long long chunkDelta = -floor(location.y);
			location.y += chunkDelta;
			chunkLocation.y -= chunkDelta;
		}
	}

	if (floor(location.z) != 0) {
		if (floor(location.z) > 0) {
			unsigned long long chunkDelta = floor(location.z);

			if (chunkLocation.z + chunkDelta > chunkLocation.z) {
				location.z -= chunkDelta;
				chunkLocation.z += chunkDelta;
			}
			else {
				//camera is outside world bounds, but we will render anyway
				chunkDelta = ULLONG_MAX - chunkLocation.z;
				chunkLocation.z = ULLONG_MAX;
				location.z -= chunkDelta;
			}
		}
		else {

			unsigned long long chunkDelta = -floor(location.z);

			if (chunkDelta > chunkLocation.z) {
				chunkLocation.z = 0;
				location.z = 0;
			}
			else {
				location.z += chunkDelta;
				chunkLocation.z -= chunkDelta;
			}
		}
	}
}
void Camera::processLocation(Time& renderTime)
{
	cameraSpeed = pow(CAMERA_SPEED_MULTIPLIER, speedMultiplier);
	accelerationVector *= cameraSpeed * renderTime.delta;

	location += accelerationVector;

	applySubChunkLocation();

	accelerationVector = { 0,0,0 };
}
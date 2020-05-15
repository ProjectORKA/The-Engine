
#include "Camera.hpp"
#include "Renderer.hpp"

struct Viewport;

void applySubChunkLocation(OctreeWorldSystemCamera& camera) {
	if ((camera.chunkLocation.x > 1) | (camera.chunkLocation.x <= -1)) {
		LL x = camera.location.x;
		camera.location.x -= x;
		camera.chunkLocation.x += x;
	}

	if ((camera.location.y > 1) | (camera.location.y <= -1)) {
		LL y = camera.location.y;
		camera.location.y -= y;
		camera.chunkLocation.y += y;
	}

	Float lowest = 0.99;

	if ((camera.location.z < -lowest) & (camera.chunkLocation.z == 0)) {
		camera.location.z = -lowest;
		camera.chunkLocation.z = 0;
	}
	else {
		if (camera.location.z < -1) {
			ULL intDeltaZ = 0;
			intDeltaZ -= camera.location.z;
			ULL newZ = camera.chunkLocation.z - intDeltaZ;

			if (newZ > camera.chunkLocation.z) {
				camera.chunkLocation.z = 0;
				camera.location.z = -lowest;
			}
			else {
				camera.location.z += intDeltaZ;
				camera.chunkLocation.z = newZ;
			}
		}
		if (camera.location.z > 1) {
			ULL maximum = ULLONG_MAX / 2;
			ULL intDeltaZ = camera.location.z;
			ULL newZ = camera.chunkLocation.z + intDeltaZ;
			if ((newZ < camera.chunkLocation.z) | (newZ > maximum)) {
				intDeltaZ = maximum - camera.chunkLocation.z;
				camera.chunkLocation.z = maximum;
				camera.location -= intDeltaZ;
			}
			else {
				camera.location.z -= intDeltaZ;
				camera.chunkLocation.z = newZ;
			}
		}
	}
}

void CameraSystem::create()
{
	add();
	select(0);
	current().rotate(0,0);
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
	
}
Camera& CameraSystem::current()
{
	return cameras[currentCamera];
}
void CameraSystem::destroy()
{
	cameras.clear();
}

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
void Camera::processLocation(Time& renderTime) {
	cameraSpeed = pow(CAMERA_SPEED_MULTIPLIER, speedMultiplier);
	accelerationVector *= cameraSpeed * renderTime.delta;

	location += glm::clamp(accelerationVector, (Float)LLONG_MIN / 2, (Float)LLONG_MAX / 2);

	accelerationVector = { 0,0,0 };
}

void OctreeWorldSystemCamera::process(Time & renderTime)
{
	cameraSpeed = pow(CAMERA_SPEED_MULTIPLIER, speedMultiplier);
	accelerationVector *= cameraSpeed * renderTime.delta;

	location += glm::clamp(accelerationVector, (Float)LLONG_MIN / 2, (Float)LLONG_MAX / 2);

	applySubChunkLocation(*this);

	accelerationVector = { 0,0,0 };
}
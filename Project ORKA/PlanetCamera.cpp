
#include "PlanetCamera.hpp"

void PlanetCamera::update(Time& renderTime)
{
	cameraSpeed = pow(CAMERA_SPEED_MULTIPLIER, speedMultiplier);
	accelerationVector *= cameraSpeed * renderTime.delta;

	location += accelerationVector;

	//apply sub chunk location
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

	accelerationVector = { 0,0,0 };
}
void PlanetCamera::rotate(float x, float y) {
	cameraRotationX -= y;
	cameraRotationZ += x;

	//prevent looking upside down
	Float cap = PI / 2;

	if (cameraRotationX < -cap) {
		cameraRotationX = -cap;
	}
	if (cameraRotationX > +cap) {
		cameraRotationX = +cap;
	}

	//calculate directional vectors
	forwardVector = Vec3(
		cos(cameraRotationX) * sin(cameraRotationZ),
		cos(cameraRotationX) * cos(cameraRotationZ),
		sin(cameraRotationX)
	);

	rightVector = Vec3(
		-sin(cameraRotationZ - PI / 2),
		-cos(cameraRotationZ - PI / 2),
		0
	);

	upVector = glm::cross(rightVector, forwardVector);
}
void PlanetCamera::render(Uniforms & uniforms, Viewport & currentViewport) {
	Float cameraHeight = chunkLocation.z + location.z;
	uniforms.setFloat("cameraHeight", cameraHeight);
	uniforms.setVec3("cameraVector", forwardVector);
	uniforms.setMatrix("vpMatrix", projectionMatrix(currentViewport.aspectRatio()) * viewMatrixOnlyRot());
}

Matrix PlanetCamera::viewMatrix() {
	return glm::lookAt(
		location,
		location + forwardVector,
		upVector
	);
}
Matrix PlanetCamera::viewMatrixOnlyRot() {
	return glm::lookAt(
		Vec3(0),
		forwardVector,
		upVector
	);
}
Matrix PlanetCamera::projectionMatrix(float aspectRatio)
{
	return glm::perspective(
		glm::radians(fieldOfView),
		aspectRatio,
		nearClipValue,
		farClipValue
	);
}

#include "Program.hpp"

void rotateCamera(Camera & camera, float x, float y) {
	camera.cameraRotationX -= camera.mouseSensitivity * y;
	camera.cameraRotationZ += camera.mouseSensitivity * x;

	//prevent looking upside down
	float cap = PI / 2;

	if (camera.cameraRotationX < -cap) {
		camera.cameraRotationX = -cap;
	}
	if (camera.cameraRotationX > +cap) {
		camera.cameraRotationX = +cap;
	}

	//calculate directional vectors
	camera.forwardVector = glm::vec3(
		cos(camera.cameraRotationX) * sin(camera.cameraRotationZ),
		cos(camera.cameraRotationX) * cos(camera.cameraRotationZ),
		sin(camera.cameraRotationX)
	);

	camera.rightVector = glm::vec3(
		-sin(camera.cameraRotationZ - PI / 2),
		-cos(camera.cameraRotationZ - PI / 2),
		0
	);

	camera.upVector = glm::cross(camera.rightVector, camera.forwardVector);

	camera.viewMatrix = glm::lookAt(glm::vec3(0), glm::vec3(0) + camera.forwardVector, camera.upVector);
}

void applySubChunkLocation(Camera& camera) {
	if ((camera.subChunkLocation.x > 1) | (camera.subChunkLocation.x <= -1)) {
		long long x = camera.subChunkLocation.x;
		camera.subChunkLocation.x -= x;
		camera.location.x += x;
	}

	if ((camera.subChunkLocation.y > 1) | (camera.subChunkLocation.y <= -1)) {
		long long y = camera.subChunkLocation.y;
		camera.subChunkLocation.y -= y;
		camera.location.y += y;
	}


	if ((camera.subChunkLocation.z < -0.5) & (camera.location.z == 0)) {
		camera.subChunkLocation.z = -0.5;
		camera.location.z = 0;
	}
	else {
		if (camera.subChunkLocation.z < -1) {
			unsigned long long intDeltaZ = 0;
			intDeltaZ -= camera.subChunkLocation.z;
			unsigned long long newZ = camera.location.z - intDeltaZ;

			if (newZ > camera.location.z) {
				camera.location.z = 0;
				camera.subChunkLocation.z = -0.5;
			}
			else {
				camera.subChunkLocation.z += intDeltaZ;
				camera.location.z = newZ;
			}
		}
		if (camera.subChunkLocation.z > 1) {
			unsigned long long maximum = ULLONG_MAX / 2;
			unsigned long long intDeltaZ = camera.subChunkLocation.z;
			unsigned long long newZ = camera.location.z + intDeltaZ;
			if ((newZ < camera.location.z) | (newZ > maximum)) {
				intDeltaZ = maximum - camera.location.z;
				camera.location.z = maximum;
				camera.subChunkLocation -= intDeltaZ;
			}
			else {
				camera.subChunkLocation.z -= intDeltaZ;
				camera.location.z = newZ;
			}
		}
	}
}

void pocessCamera(Camera & camera, Time & time)
{
	camera.cameraSpeed = (1.0f + float(camera.location.z)) * pow(CAMERA_SPEED_MULTIPLIER, camera.speedMultiplier);
	camera.accelerationVector *= camera.cameraSpeed * time.delta();
	
	camera.subChunkLocation += glm::clamp(camera.accelerationVector, (float)LLONG_MIN / 2, (float)LLONG_MAX / 2);

	applySubChunkLocation(camera);

	camera.accelerationVector = { 0,0,0 };
}
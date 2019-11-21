
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

void pocessCamera(Camera & camera, Time & time)
{
	//calculate acceleration
	camera.accelerationVector *= camera.cameraSpeed * time.getDelta();
	
	//add acceleration to floating point position vector
	camera.subChunkLocation += camera.accelerationVector;

	//transfer floating point delta to integer position if bigger than 1
	if (camera.subChunkLocation.x >= 1 | camera.subChunkLocation.x <= -1) {
		long long x = camera.subChunkLocation.x;
		camera.subChunkLocation.x -= x;
		camera.location.x += x;
	}

	if (camera.subChunkLocation.y >= 1 | camera.subChunkLocation.y <= -1) {
		long long y = camera.subChunkLocation.y;
		camera.subChunkLocation.y -= y;
		camera.location.y += y;
	}

	if (camera.subChunkLocation.z <= -1) {
		unsigned long long intDeltaZ = 0 - camera.subChunkLocation.z;
		unsigned long long newZ = camera.location.z - intDeltaZ;
		if (newZ > camera.location.z) {
			intDeltaZ = camera.location.z;
			camera.location.z = 0;
			camera.subChunkLocation += intDeltaZ;
		}
		else {
			camera.subChunkLocation.z += intDeltaZ;
			camera.location.z = newZ;
		}
	}

	if (camera.subChunkLocation.z >= 1) {
		unsigned long long intDeltaZ = camera.subChunkLocation.z;
		unsigned long long newZ = camera.location.z + intDeltaZ;
		if (newZ < camera.location.z) {
			intDeltaZ = ULLONG_MAX - camera.location.z;
			camera.location.z = ULLONG_MAX;
			camera.subChunkLocation -= intDeltaZ;
		}
		else {
			camera.subChunkLocation.z -= intDeltaZ;
			camera.location.z = newZ;
		}
	}

	//reset acceleration vector
	camera.accelerationVector = { 0,0,0 };
}
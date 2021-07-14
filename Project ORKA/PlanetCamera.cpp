
#include "PlanetCamera.hpp"
#include "TerrainSystem.hpp"

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

	if (CAMERA_TERRAIN_LIMIT) {
		LDouble oldCamHeight = chunkLocation.z + location.z;

		LDouble xLocation = location.x;
		LDouble yLocation = location.y;

		xLocation += LDouble(chunkLocation.x) / pow(2, 64);
		yLocation += LDouble(chunkLocation.y) / pow(2, 64);

		//xLocation /= pow(2, -3);
		//yLocation /= pow(2, -3);

		LDouble newCamHeight = pow(2, 64-17) + terrainGenerationFunction(xLocation, yLocation);

		if (newCamHeight > oldCamHeight) {
			ULL chunkheight = newCamHeight;
			newCamHeight -= chunkheight;
			chunkLocation.z = chunkheight;
			location.z = newCamHeight;
		}
	}
}
void PlanetCamera::render(Uniforms& uniforms, Float aspectRatio) {
	Float cameraHeight = chunkLocation.z + location.z;
	uniforms.data.custom1 = cameraHeight;
	uniforms.data.cameraVector = Vec4(forwardVector, 1);
	uniforms.data.vpMatrix = projectionMatrix(aspectRatio) * viewMatrixOnlyRot();
	//uniforms.update();
}
Matrix PlanetCamera::viewMatrixOnlyRot() {
	return glm::lookAt(
		Vec3(0),
		forwardVector,
		upVector
	);
}
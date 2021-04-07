#include "SimpleCamera.hpp"

void SimpleCamera::rotate(Float mouseDeltaX, Float mouseDeltaY)
{
	rotationX -= mouseDeltaY;
	rotationZ += mouseDeltaX;

	//prevent looking upside down
	Float cap = PI / 2;

	if (rotationX < -cap) {
		rotationX = -cap;
	}
	if (rotationX > +cap) {
		rotationX = +cap;
	}

	//calculate directional vectors
	forwardVector = Vec3(
		cos(rotationX) * sin(rotationZ),
		cos(rotationX) * cos(rotationZ),
		sin(rotationX)
	);

	rightVector = Vec3(
		-sin(rotationZ - PI / 2),
		-cos(rotationZ - PI / 2),
		0
	);

	upVector = glm::cross(rightVector, forwardVector);
}
Matrix SimpleCamera::viewMatrix() {
	return glm::lookAt(
		location,
		location + forwardVector,
		upVector
	);
}
Matrix SimpleCamera::projectionMatrix(float aspectRatio)
{
	return glm::perspective(
		glm::radians(fieldOfView),
		aspectRatio,
		nearClipValue,
		farClipValue
	);
}
void SimpleCamera::setLocation(Vec3 location) {
	this->location = location;
}
#include "SimpleCamera.hpp"

void SimpleCamera::update(Float delta) {
	accelerationVector *= delta;
	location += accelerationVector;
	accelerationVector = { 0,0,0 };
}
void SimpleCamera::rotate(Vec2 rotation) {
	rotationX -= rotation.y;
	rotationZ += rotation.x;

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
void SimpleCamera::render(Uniforms& uniforms, Float aspectRatio) {
	uniforms.data.cameraVector = Vec4(forwardVector, 1);
	uniforms.data.vpMatrix = projectionMatrix(aspectRatio) * viewMatrix();
	uniforms.update();
}

Matrix SimpleCamera::viewMatrix() {
	return glm::lookAt(
		location,
		location + forwardVector,
		upVector
	);
}
Matrix SimpleCamera::viewMatrixOnlyRot() {
	return glm::lookAt(
		Vec3(0),
		forwardVector,
		upVector
	);
}
Matrix SimpleCamera::projectionMatrix(Float aspectRatio) {
	return glm::perspective(
		glm::radians(fieldOfView),
		aspectRatio,
		nearClipValue,
		farClipValue);
}
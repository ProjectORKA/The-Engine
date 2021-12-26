#include "Camera.hpp"
#include "Renderer.hpp"

void Camera::rotate(Vec2 rotation) {
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
void Camera::render(Renderer & renderer) {
	renderer.uniforms().data.cameraVector = Vec4(forwardVector, 1);
	renderer.uniforms().data.cameraPosition = Vec4(location,1);
	renderer.uniforms().data.vMatrix = viewMatrix();
	renderer.uniforms().data.pMatrix = projectionMatrix(renderer.aspectRatio());
	renderer.uniforms().update();
}
void Camera::renderOnlyRot(Renderer& renderer)
{
	renderer.uniforms().data.cameraVector = Vec4(forwardVector, 1);
	renderer.uniforms().data.cameraPosition = Vec4(location, 1);
	renderer.uniforms().data.vMatrix = viewMatrixOnlyRot();
	renderer.uniforms().data.pMatrix = projectionMatrix(renderer.aspectRatio());
	renderer.uniforms().update();
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
Matrix Camera::projectionMatrix(Float aspectRatio) {
	return glm::perspective(
		glm::radians(fieldOfView),
		aspectRatio,
		nearClipValue,
		farClipValue);
}
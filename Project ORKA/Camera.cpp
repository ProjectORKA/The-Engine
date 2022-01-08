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
	renderer.uniforms().cameraVec() = Vec4(forwardVector, 1);
	renderer.uniforms().cameraPos() = Vec4(location, 1);
	renderer.uniforms().vMatrix() = viewMatrix();
	renderer.uniforms().pMatrix() = projectionMatrix(renderer.aspectRatio());
}
void Camera::renderOnlyRot(Renderer& renderer)
{
	renderer.uniforms().cameraVec() = Vec4(forwardVector, 1);
	renderer.uniforms().cameraPos() = Vec4(location, 1);
	renderer.uniforms().vMatrix() = viewMatrixOnlyRot();
	renderer.uniforms().pMatrix() = projectionMatrix(renderer.aspectRatio());
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
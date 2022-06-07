#include "Camera.hpp"

DVec3 Camera::getRotation() {
	return rotation;
}

Double Camera::getRotationZ() {
	return rotation.z;
}

Double Camera::getRotationX() {
	return rotation.x;
}

void Camera::update() {
	//prevent looking upside down
	const Float cap = PI/2;
	if (rotation.x < -cap) {rotation.x = -cap;} else
	if (rotation.x > cap) { rotation.x = cap;}

	//figure out direction based on z rotation
	DVec2 direction = DVec2(sin(rotation.z), cos(rotation.z)); //direction were facing
	DVec2 direction2 = DVec2(direction.y, -direction.x);		 //90° right of direction

	//complicated math stuff
	forwardVector = normalize(DVec3(
		direction.x * cos(-rotation.x),
		direction.y * cos(-rotation.x),
		sin(-rotation.x)));
	//more complicated math stuff
	rightVector = normalize(DVec3(
		direction2.x * cos(rotation.y),
		direction2.y * cos(rotation.y),
		sin(rotation.y)
	));
	//calculate up from existing info
	upVector = normalize(cross(rightVector, forwardVector));
}
void Camera::rotate(DVec2 rotation) {
	this->rotation.z += rotation.x;
	this->rotation.x -= rotation.y;

	update();
}
void Camera::render(Renderer & renderer) {
	renderer.uniforms().cameraVec() = Vec4(forwardVector, 1);
	renderer.uniforms().cameraPos() = Vec4(location, 1);
	renderer.uniforms().vMatrix() = viewMatrix();
	renderer.uniforms().pMatrix() = projectionMatrix(renderer.getAspectRatio());
}
void Camera::setRotation(DVec3 rotation) {
	this->rotation = rotation;
	update();
};
void Camera::renderOnlyRot(Renderer& renderer)
{
	renderer.uniforms().cameraVec() = Vec4(forwardVector, 1);
	renderer.uniforms().cameraPos() = Vec4(location, 1);
	renderer.uniforms().vMatrix() = viewMatrixOnlyRot();
	renderer.uniforms().pMatrix() = projectionMatrix(renderer.getAspectRatio());
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
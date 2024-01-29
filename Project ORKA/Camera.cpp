#include "Camera.hpp"

void Camera::update()
{
	// prevent looking upside down
	constexpr Float cap = PI / 2.0f;
	if(rotation.x < -cap) rotation.x = -cap;
	else if(rotation.x > cap) rotation.x = cap;

	// figure out direction based on z rotation
	const auto direction  = DVec2(sin(rotation.z), cos(rotation.z));	// direction were facing
	const auto direction2 = DVec2(direction.y, -direction.x);			// 90° right of direction

	// complicated math stuff
	forwardVector = normalize(DVec3(direction.x * cos(-rotation.x), direction.y * cos(-rotation.x), sin(-rotation.x)));
	// more complicated math stuff
	rightVector = normalize(DVec3(direction2.x * cos(rotation.y), direction2.y * cos(rotation.y), sin(rotation.y)));
	// calculate up from existing info
	upVector = normalize(cross(rightVector, forwardVector));
}

Vec3 Camera::getLocation() const
{
	return location;
}

Vec3 Camera::getUpVector() const
{
	return upVector;
}

DVec3 Camera::getRotation() const
{
	return rotation;
}

Matrix Camera::viewMatrix() const
{
	return lookAt(location, location + forwardVector, upVector);
}

Double Camera::getRotationZ() const
{
	return rotation.z;
}

Double Camera::getRotationX() const
{
	return rotation.x;
}

Vec3 Camera::getRightVector() const
{
	return rightVector;
}

Float Camera::getFieldOfView() const
{
	return fieldOfView;
}

Float Camera::getFarClipValue() const
{
	return farClipValue;
}

Vec3 Camera::getForwardVector() const
{
	return forwardVector;
}

Float Camera::getNearClipValue() const
{
	return nearClipValue;
}

Matrix Camera::viewMatrixOnlyRot() const
{
	return lookAt(Vec3(0), forwardVector, upVector);
}

void Camera::rotate(const DVec2 rotation)
{
	this->rotation.z += rotation.x;
	this->rotation.x -= rotation.y;

	update();
}

void Camera::render(Renderer& renderer) const
{
	renderer.uniforms().setCameraVec(Vec4(forwardVector, 1));
	renderer.uniforms().setCameraPos(Vec4(location, 1));
	renderer.uniforms().setVMatrix(viewMatrix());
	renderer.uniforms().setPMatrix(projectionMatrix(renderer.getAspectRatio()));
}

void Camera::setLocation(const Vec3 location)
{
	this->location = location;
}

void Camera::setRotation(const DVec3& rotation)
{
	this->rotation = rotation;
	update();
}

void Camera::renderOnlyRot(Renderer& renderer) const
{
	renderer.uniforms().setCameraVec(Vec4(forwardVector, 1));
	renderer.uniforms().setCameraPos(Vec4(location, 1));
	renderer.uniforms().setVMatrix(viewMatrixOnlyRot());
	renderer.uniforms().setPMatrix(projectionMatrix(renderer.getAspectRatio()));
}

void Camera::setFieldOfView(const Float fieldOfView)
{
	this->fieldOfView = fieldOfView;
}

void Camera::setFarClipValue(const Float farClipValue)
{
	this->farClipValue = farClipValue;
}

void Camera::setNearClipValue(const Float nearClipValue)
{
	this->nearClipValue = nearClipValue;
}

Matrix Camera::projectionMatrix(const Float aspectRatio) const
{
	return glm::perspective(glm::radians(fieldOfView), aspectRatio, nearClipValue, farClipValue);
}

void Camera::setLocation(const Float x, const Float y, const Float z)
{
	this->location = Vec3(x, y, z);
}

void Camera::setRotation(const Double x, const Double y, const Double z)
{
	this->rotation = DVec3(x, y, z);
	update();
}

glm::mat4 perspective2(const Float fov, const Float aspectRatio, const Float near, const Float far)
{
	// Calculate the half vertical field of view in radians
	const Float halfFOV = glm::radians(fov) / 2.0f;

	// Calculate the distance to the projection plane
	const Float distance = 1.0f / std::tan(halfFOV);

	// Calculate the values for the perspective projection matrix
	const Float a = distance / aspectRatio;
	const Float b = distance;
	const Float c = -(far + near) / (far - near);
	const Float d = -(2.0f * far * near) / (far - near);

	// Construct the perspective projection matrix
	glm::mat4 projectionMatrix(0.0f);
	projectionMatrix[0][0] = a;
	projectionMatrix[1][1] = b;
	projectionMatrix[2][2] = c;
	projectionMatrix[2][3] = -1.0f;
	projectionMatrix[3][2] = d;

	return projectionMatrix;
}

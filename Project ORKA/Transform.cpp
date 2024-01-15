#include "Transform.hpp"
#include "Renderer.hpp"

Transform::Transform() = default;

Vec3 Transform::getSize() const
{
	return size;
}

void Transform::setX(const Float x)
{
	position.x = x;
}

void Transform::setY(const Float y)
{
	position.y = y;
}

void Transform::setZ(const Float z)
{
	position.z = z;
}

Vec3 Transform::getLocation() const
{
	return position;
}

Vec3 Transform::getRotation() const
{
	return degrees(eulerAngles(rotation));
}

Matrix Transform::getMatrix() const
{
	Matrix matrix = Matrix(1.0f);
	matrix        = translate(matrix, position);
	matrix        = matrix * mat4_cast(rotation);
	matrix        = scale(matrix, size);
	return matrix;
}

void Transform::setSize(const Float size)
{
	this->size = Vec3(size);
}

void Transform::setSize(const Vec3& size)
{
	this->size = size;
}

Transform::Transform(const Vec3 position)
{
	this->position = position;
	this->rotation = Quaternion();
	this->size     = Vec3(1);
}

void Transform::move(const Vec3 translation)
{
	position += translation;
}

void Transform::setLocation(const Vec3& pos)
{
	position = pos;
}

void Transform::render(Renderer& renderer) const
{
	renderer.uniforms().setMMatrix(getMatrix());
}

void Transform::scaleBy(const Vec3& scaleFactor)
{
	size *= scaleFactor;
}

void Transform::setRotation(const Vec3& eulerAngles)
{
	rotation = Quaternion(radians(eulerAngles));
}

void Transform::rotate(const Vec3& axis, const Float angle)
{
	const Quaternion rotationChange = angleAxis(glm::radians(angle), normalize(axis));
	rotation                        = rotationChange * rotation;
}

void Transform::setSize(const Float x, const Float y, const Float z)
{
	setSize(Vec3(x, y, z));
}

void Transform::setLocation(const Float x, const Float y, const Float z)
{
	setLocation(Vec3(x, y, z));
}

void Transform::setRotation(const Float x, const Float y, const Float z)
{
	setRotation(Vec3(x, y, z));
}

Transform::Transform(const Vec3 position, const Vec3 rotation, const Vec3 size)
{
	this->position = position;
	this->rotation = Quaternion(radians(rotation));
	this->size     = size;
}

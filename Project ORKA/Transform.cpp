#include "Transform.hpp"
#include "Renderer.hpp"

Matrix Transform::matrix() const
{
	auto m = Matrix(1);
	m = translate(m, location);
	m = toMat4(Quaternion(rotation)) * m;
	m = glm::scale(m, scale);
	return m;
}

void Transform::render(Renderer& renderer) const
{
	renderer.uniforms().mMatrix() = matrix();
}
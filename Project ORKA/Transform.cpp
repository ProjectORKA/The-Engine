
#include "Transform.hpp"
#include "Renderer.hpp"

Matrix Transform::matrix() {
	Matrix m = Matrix(1);
	m = glm::translate(m, location);
	m = glm::toMat4(Quat(rotation)) * m;
	m = glm::scale(m, Vec3(scale));
	return m;
}

void Transform::render(Renderer& renderer) {
	renderer.uniforms().mMatrix() = matrix();
}
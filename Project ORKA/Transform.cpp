
#include "Transform.hpp"
#include "Renderer.hpp"

Matrix Transform::matrix() {
	Matrix m = Matrix(1);
	m = translate(m, location);
	m = glm::toMat4(Quat(rotation)) * m;
	return m;
}

void Transform::render(Renderer& renderer) {
	renderer.uniforms().data.mMatrix = matrix();
}
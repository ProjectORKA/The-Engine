
#include "MatrixSystem.hpp"

Matrix MatrixSystem::getVP() {
	return getProjection() * getView();
}
Matrix MatrixSystem::getMVP() {
	return  getProjection() * getView() * getModel();
}
Matrix MatrixSystem::getView() {
	return viewMatrix;
}
Matrix MatrixSystem::getModel() {
	return transformationStack.get();
}
Matrix MatrixSystem::getProjection() {
	return projectionMatrix;
}

UInt MatrixSystem::stackCount() {
	return transformationStack.matrices.size();
}

void MatrixSystem::popMatrix() {
	transformationStack.popMatrix();
}
void MatrixSystem::setView(Matrix matrix) {
	viewMatrix = matrix;
}
void MatrixSystem::pushMatrix(Matrix matrix) {
	transformationStack.pushMatrix(matrix);
}
void MatrixSystem::overridePush(Matrix matrix) {
	transformationStack.overridePush(matrix);
}
void MatrixSystem::setProjection(Matrix matrix) {
	projectionMatrix = matrix;
}
void MatrixSystem::applyToUniforms(Uniforms& uniforms) {
	uniforms.data.mMatrix = getModel();
	uniforms.data.vpMatrix = getVP();
}
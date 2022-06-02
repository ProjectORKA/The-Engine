
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
void MatrixSystem::matrixArray(Vector<Vec2>& pos) {
	modelMatrixArray.resize(pos.size());
	for (UInt i = 0; i < pos.size(); i++) {
		modelMatrixArray[i] = matrixFromLocation(pos[i]);
	}
}
void MatrixSystem::matrixArray(Vector<Vec2>& pos, Float size) {
	modelMatrixArray.resize(pos.size());
	for (UInt i = 0; i < pos.size(); i++) {
		modelMatrixArray[i] = matrixFromLocationAndSize(pos[i], size);
	}
}
void MatrixSystem::matrixArray(Vector<Vec2>& pos, Vector<Vec2>& dir) {
	modelMatrixArray.resize(pos.size());
	for (UInt i = 0; i < pos.size(); i++) {
		modelMatrixArray[i] = matrixFromPositionAndDirection(pos[i], dir[i]);
	}
}
void MatrixSystem::matrixArray(Vector<Vec4>& compactTransform) {
	modelMatrixArray.resize(compactTransform.size());
	for (UInt i = 0; i < compactTransform.size(); i++) {
		modelMatrixArray[i] = matrixFromLocationAndSize(compactTransform[i]);
	}
}
void MatrixSystem::matrixArray(Vector<Vec2>& position, Vector<Vec2>& direction, Vector<Float>& size) {
	modelMatrixArray.resize(position.size());
	for (UInt i = 0; i < position.size(); i++) {
		modelMatrixArray[i] = matrixFromLocationDirectionAndSize(position[i], direction[i], size[i]);
	}
}
void MatrixSystem::applyToUniforms(Uniforms& uniforms) {
	uniforms.mMatrix() = getModel();
	uniforms.vMatrix() = getView();
	uniforms.pMatrix() = getProjection();
}
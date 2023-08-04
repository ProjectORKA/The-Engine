#include "MatrixSystem.hpp"
#include "Math.hpp"

void MatrixSystem::popMatrix()
{
	transformationStack.popMatrix();
}

Matrix MatrixSystem::getVP() const
{
	return getProjection() * getView();
}

Matrix MatrixSystem::getMVP() const
{
	return getProjection() * getView() * getModel();
}

Matrix MatrixSystem::getView() const
{
	return viewMatrix;
}

Matrix MatrixSystem::getModel() const
{
	return transformationStack.get();
}

UInt MatrixSystem::stackCount() const
{
	return transformationStack.matrices.size();
}

Matrix MatrixSystem::getProjection() const
{
	return projectionMatrix;
}

void MatrixSystem::setView(const Matrix& matrix)
{
	viewMatrix = matrix;
}

void MatrixSystem::pushMatrix(const Matrix& matrix)
{
	transformationStack.pushMatrix(matrix);
}

void MatrixSystem::overridePush(const Matrix& matrix)
{
	transformationStack.overridePush(matrix);
}

void MatrixSystem::setProjection(const Matrix& matrix)
{
	projectionMatrix = matrix;
}

void MatrixSystem::matrixArray(const Vector<Vec2>& pos)
{
	modelMatrixArray.resize(pos.size());
	for(UInt i = 0; i < pos.size(); i++) modelMatrixArray[i] = matrixFromLocation(pos[i]);
}

void MatrixSystem::applyToUniforms(Uniforms& uniforms) const
{
	uniforms.setMMatrix(getModel());
	uniforms.setVMatrix(getView());
	uniforms.setPMatrix(getProjection());
}

void MatrixSystem::matrixArray(const Vector<Vec4>& compactTransform)
{
	modelMatrixArray.resize(compactTransform.size());
	for(UInt i = 0; i < compactTransform.size(); i++) modelMatrixArray[i] = matrixFromLocationAndSize(compactTransform[i]);
}

void MatrixSystem::matrixArray(const Vector<Vec2>& pos, const Float size)
{
	modelMatrixArray.resize(pos.size());
	for(UInt i = 0; i < pos.size(); i++) modelMatrixArray[i] = matrixFromLocationAndSize(pos[i], size);
}

void MatrixSystem::matrixArray(const Vector<Vec2>& pos, const Vector<Vec2>& dir)
{
	modelMatrixArray.resize(pos.size());
	for(UInt i = 0; i < pos.size(); i++) modelMatrixArray[i] = matrixFromPositionAndDirection(pos[i], dir[i]);
}

void MatrixSystem::matrixArray(const Vector<Vec2>& position, const Vector<Vec2>& direction, const Vector<Float>& size)
{
	modelMatrixArray.resize(position.size());
	for(UInt i = 0; i < position.size(); i++) modelMatrixArray[i] = matrixFromLocationDirectionAndSize(position[i], direction[i], size[i]);
}

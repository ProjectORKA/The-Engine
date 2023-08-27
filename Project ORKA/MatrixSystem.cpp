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

void MatrixSystem::applyToUniforms(Uniforms& uniforms) const
{
	uniforms.setMMatrix(getModel());
	uniforms.setVMatrix(getView());
	uniforms.setPMatrix(getProjection());
}

#include "TransformationStack.hpp"

void TransformationStack::popMatrix()
{
	matrices.pop_back();
}

Matrix TransformationStack::get() const
{
	if(matrices.empty()) return Matrix(1);
	return matrices.back();
}

void TransformationStack::pushMatrix(const Matrix& matrix)
{
	matrices.push_back(matrix * get());
}

void TransformationStack::overridePush(const Matrix& matrix)
{
	matrices.push_back(matrix);
}

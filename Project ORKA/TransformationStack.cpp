
#include "TransformationStack.hpp"

Matrix TransformationStack::get() {
	if (matrices.size() == 0) return Matrix(1);
	else return matrices.back();
}
void TransformationStack::popMatrix() {
	matrices.pop_back();
}
void TransformationStack::pushMatrix(Matrix matrix) {
	matrices.push_back(matrix * get());
}
void TransformationStack::overridePush(Matrix matrix) {
	matrices.push_back(matrix);
}
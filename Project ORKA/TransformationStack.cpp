
#include "TransformationStack.hpp"

Matrix TransformationStack::get() {
	if (matrices.size() == 0) return Matrix(1);
	else return matrices.last();
}
void TransformationStack::popMatrix() {
	matrices.popBack();
}
void TransformationStack::pushMatrix(Matrix matrix) {
	matrices.pushBack(matrix * get());
}
void TransformationStack::overridePush(Matrix matrix) {
	matrices.pushBack(matrix);
}
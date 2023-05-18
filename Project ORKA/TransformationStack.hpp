#pragma once

#include "Basics.hpp"

struct TransformationStack {
	Vector<Matrix> matrices;

	Matrix get() const;
	void popMatrix();
	void pushMatrix(const Matrix& matrix);
	void overridePush(const Matrix& matrix);
};

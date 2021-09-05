#pragma once

#include "Basics.hpp"

struct TransformationStack {
	Vector<Matrix> matrices;

	Matrix get();
	void popMatrix();
	void pushMatrix(Matrix matrix);
	void overridePush(Matrix matrix);
};
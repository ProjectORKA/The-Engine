#pragma once

#include "TransformationStack.hpp"
#include "Uniforms.hpp"
#include "Math.hpp"

// the transformation stack is not working yet, or at least its not being used anywhere
// but matrix arrays are very useful for instanced rendering
// just give it the information you have and it will render instanced meshes based on that
struct MatrixSystem {
	//matrix array for instanced rendering
	Vector<Matrix> modelMatrixArray;
	void matrixArray(Vector<Vec2>& pos);
	void matrixArray(Vector<Vec2>& pos, Float size);
	void matrixArray(Vector<Vec4>& compactTransform);
	void matrixArray(Vector<Vec2>& pos, Vector<Vec2>& dir);
	void matrixArray(Vector<Vec2>& position, Vector<Vec2>& direction, Vector<Float>& size);

	//getters
	Matrix getVP();
	Matrix getMVP();
	Matrix getView();
	Matrix getModel();
	Matrix getProjection();

	//setters
	void setView(Matrix matrix);
	void setProjection(Matrix matrix);
	//model matrices are stacked
	void popMatrix();
	UInt stackCount();
	void pushMatrix(Matrix matrix);
	void overridePush(Matrix matrix);
	void applyToUniforms(Uniforms& uniforms);

private :
	TransformationStack transformationStack;
	Matrix mvp = Matrix(1);
	Matrix viewMatrix = Matrix(1);
	Matrix projectionMatrix =Matrix(1);
};
#pragma once

#include "TransformationStack.hpp"
#include "Uniforms.hpp"
#include "Math.hpp"

struct MatrixSystem {
	Vector<Matrix> modelMatrixArray; // experimental system for instancing

	void matrixArray(Vector<Vec2>& pos) {
		modelMatrixArray.resize(pos.size());
		for (UInt i = 0; i < pos.size(); i++) {
			modelMatrixArray[i] = matrixFromLocation(pos[i]);
		}
	};

	void matrixArray(Vector<Vec2> & pos, Vector<Vec2> & dir) {
		modelMatrixArray.resize(pos.size());
		for (UInt i = 0; i < pos.size(); i++) {
			modelMatrixArray[i] = matrixFromPositionAndDirection(pos[i],dir[i]);
		}
	};

	void matrixArray(Vector<Vec4>& compactTransform) {
		modelMatrixArray.resize(compactTransform.size());
		for (UInt i = 0; i < compactTransform.size(); i++) {
			modelMatrixArray[i] = matrixFromLocationAndSize(compactTransform[i]);
		}
	};

	void matrixArray(Vector<Vec2>& position, Vector<Vec2>& direction, Vector<Float>& size) {
		modelMatrixArray.resize(position.size());
		for (UInt i = 0; i < position.size(); i++) {
			modelMatrixArray[i] = matrixFromLocationDirectionAndSize(position[i], direction[i], size[i]);
		}
	}

	void applyToUniforms(Uniforms& uniforms);

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

private :
	TransformationStack transformationStack;
	Matrix viewMatrix = Matrix(1);
	Matrix projectionMatrix =Matrix(1);
	Matrix mvp = Matrix(1);
};
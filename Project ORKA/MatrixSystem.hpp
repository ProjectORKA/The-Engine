#pragma once

#include "TransformationStack.hpp"
#include "Uniforms.hpp"

// the transformation stack is not working yet, or at least its not being used anywhere
// but matrix arrays are very useful for instanced rendering
// just give it the information you have and it will render instanced meshes based on that
struct MatrixSystem
{
	// matrix array for instanced rendering
	Vector<Matrix> modelMatrixArray;
	void           matrixArray(const Vector<Vec2>& pos);
	void           matrixArray(const Vector<Vec2>& pos, Float size);
	void           matrixArray(const Vector<Vec4>& compactTransform);
	void           matrixArray(const Vector<Vec2>& pos, const Vector<Vec2>& dir);
	void           matrixArray(const Vector<Vec2>& position, const Vector<Vec2>& direction, const Vector<Float>& size);

	// getters
	[[nodiscard]] Matrix getVP() const;
	[[nodiscard]] Matrix getMVP() const;
	[[nodiscard]] Matrix getView() const;
	[[nodiscard]] Matrix getModel() const;
	[[nodiscard]] Matrix getProjection() const;

	// setters
	void setView(const Matrix& matrix);
	void setProjection(const Matrix& matrix);
	// model matrices are stacked
	void               popMatrix();
	[[nodiscard]] UInt stackCount() const;
	void               pushMatrix(const Matrix& matrix);
	void               overridePush(const Matrix& matrix);
	void               applyToUniforms(Uniforms& uniforms) const;
private :
	TransformationStack transformationStack;
	Matrix              mvp              = Matrix(1);
	Matrix              viewMatrix       = Matrix(1);
	Matrix              projectionMatrix = Matrix(1);
};

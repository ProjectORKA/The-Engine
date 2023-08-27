#pragma once

#include "TransformationStack.hpp"
#include "Uniforms.hpp"

struct MatrixSystem
{
	[[nodiscard]] Matrix getVP() const;
	[[nodiscard]] Matrix getMVP() const;
	[[nodiscard]] Matrix getView() const;
	[[nodiscard]] Matrix getModel() const;
	[[nodiscard]] Matrix getProjection() const;

	void setView(const Matrix& matrix);
	void setProjection(const Matrix& matrix);

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

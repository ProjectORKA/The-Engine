#pragma once

#include "TransformationStack.hpp"
#include "Uniforms.hpp"

struct MatrixSystem {

	void applyToUniforms(Uniforms& uniforms);;

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
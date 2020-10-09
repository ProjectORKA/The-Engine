#pragma once

#include "Basics.hpp"

enum UniformType {
	UniformBoolType,
	UniformIntType,
	UniformFloatType,
	UniformVec3Type,
	UniformVec4Type,
	UniformMatrixType
};

struct Uniforms {
	void create();
	void destroy();
	void setBool(Name name, Bool value);
	void setInt(Name name, Int value);
	void setFloat(Name name, Float value);
	void setVec3(Name name, Vec3 value);
	void setVec4(Name name, Vec4 value);
	void setMatrix(Name name, Matrix value);

	Map<Name, Bool> bools;
	Map<Name, Int> ints;
	Map<Name, Float> floats;
	Map<Name, Vec3> vec3s;
	Map<Name, Vec4> vec4s;
	Map<Name, Matrix> matrices;
};
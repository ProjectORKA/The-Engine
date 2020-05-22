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
	Map<Name, Bool> bools;
	Map<Name, Int> ints;
	Map<Name, Float> floats;
	Map<Name, Vec3> vec3s;
	Map<Name, Vec4> vec4s;
	Map<Name, Matrix> matrices;


	void create();
	void destroy();
};
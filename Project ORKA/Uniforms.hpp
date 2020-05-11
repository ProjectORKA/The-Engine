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
	Map<String, Bool> bools;
	Map<String, Int> ints;
	Map<String, Float> floats;
	Map<String, Vec3> vec3s;
	Map<String, Vec4> vec4s;
	Map<String, Matrix> matrices;


	void create();
	void destroy();
	void updateUniform(String name, Bool data);
	void updateUniform(String name, Int data);
	void updateUniform(String name, Float data);
	void updateUniform(String name, Vec3 data);
	void updateUniform(String name, Vec4 data);
	void updateUniform(String name, Matrix data);
};
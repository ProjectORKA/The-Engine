
#include "Uniforms.hpp"

void Uniforms::create()
{
	bools.clear();
	ints.clear();
	floats.clear();
	vec3s.clear();
	vec4s.clear();
	matrices.clear();

	bools["distortion"] = true;
	floats["time"] = 0.0f;
	vec3s["chunkOffset"] = Vec3(0, 0, 0);
	vec4s["worldOffset"] = Vec4(0, 0, 0, 0);
	matrices["mMatrix"] = Matrix(1);
	matrices["vpMatrix"] = Matrix(1);
}
void Uniforms::destroy()
{
	bools.clear();
	ints.clear();
	floats.clear();
	vec3s.clear();
	vec4s.clear();
	matrices.clear();
}
void Uniforms::updateUniform(String name, Int data) {
	ints[name] = data;
}
void Uniforms::updateUniform(String name, Bool data) {
	bools[name] = data;
}
void Uniforms::updateUniform(String name, Vec3 data) {
	vec3s[name] = data;
}
void Uniforms::updateUniform(String name, Vec4 data) {
	vec4s[name] = data;
}
void Uniforms::updateUniform(String name, Float data) {
	floats[name] = data;
}
void Uniforms::updateUniform(String name, Matrix data) {
	matrices[name] = data;
}
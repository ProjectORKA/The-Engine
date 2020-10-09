
#include "Uniforms.hpp"

void Uniforms::create()
{
	bools.clear();
	ints.clear();
	floats.clear();
	vec3s.clear();
	vec4s.clear();
	matrices.clear();

	bools["distortion"] = false;

	floats["time"] = 0.0f;
	floats["cameraHeight"] = 0.0f;
	
	vec3s["cameraVector"] = Vec3(0,0,-1);
	vec3s["chunkOffsetVector"] = Vec3(0, 0, 0);
	
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
void Uniforms::setBool(Name name, Bool value)
{
	bools[name] = value;
}
void Uniforms::setInt(Name name, Int value)
{
	ints[name] = value;
}
void Uniforms::setFloat(Name name, Float value)
{
	floats[name] = value;
}
void Uniforms::setVec3(Name name, Vec3 value)
{
	vec3s[name] = value;
}
void Uniforms::setVec4(Name name, Vec4 value)
{
	vec4s[name] = value;
}
void Uniforms::setMatrix(Name name, Matrix value)
{
	matrices[name] = value;
}

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
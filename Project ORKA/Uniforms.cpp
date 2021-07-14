
#include "Uniforms.hpp"

void Uniforms::create()
{
	reset();
	glGenBuffers(1, &id);
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(GlobalUniformData), &data, GL_DYNAMIC_DRAW);
	//create binding
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, id);
	
}
void Uniforms::update()
{
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(GlobalUniformData), &data, GL_DYNAMIC_DRAW);
	//[TODO] use bindSubdata to avoid uploading entire buffer, thx
}
void Uniforms::destroy()
{
	glDeleteBuffers(1, &id);
}
void Uniforms::reset()
{
	data.mMatrix = Matrix(1);
	data.vpMatrix = Matrix(1);
	data.worldOffset = Vec4(0, 0, 0, 0);
	data.cameraVector = Vec4(0, 0, 0, 0);
	data.chunkOffsetVector = Vec4(0, 0, 0, 0);
	data.customColor = Color(1, 1, 1, 1);
	data.time = 0;
	data.custom1 = 0;
	data.custom2 = 0;
	data.custom3 = 0;
	data.distortion = 0;
	data.placeholder1 = 0;
	data.placeholder2 = 0;
	data.placeholder3 = 0;

	sampler2Ds["texture0"] = 0;
}
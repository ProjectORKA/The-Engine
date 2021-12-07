
#include "Uniforms.hpp"

void Uniforms::create()
{
	reset();

	uniformBlockShaderCode = loadString("data/shaders/uniforms.glsl");

	apiGenBuffer(id);
	apiBindBuffer(GL_UNIFORM_BUFFER, id);
	apiBufferData(GL_UNIFORM_BUFFER, sizeof(GlobalUniformData), &data, GL_DYNAMIC_DRAW);
	//create binding
	apiBindBufferBase(GL_UNIFORM_BUFFER, 0, id);
	
}
void Uniforms::update()
{
	apiBindBuffer(GL_UNIFORM_BUFFER, id);
	apiBufferData(GL_UNIFORM_BUFFER, sizeof(GlobalUniformData), &data, GL_DYNAMIC_DRAW);
	//[TODO] use bindSubdata to avoid uploading entire buffer, thx
}
void Uniforms::destroy()
{
	apiDeleteBuffer(id);
}
void Uniforms::reset()
{
	data.mMatrix = Matrix(1);
	data.vpMatrix = Matrix(1);
	data.worldOffset = Vec4(0, 0, 0, 0);
	data.cameraVector = Vec4(0, 0, 0, 0);
	data.chunkOffsetVector = Vec4(0, 0, 0, 0);
	data.customColor = Vec4(1, 1, 1, 1);
	//data.time = 0.0f;		
	data.custom1 = 0.0f;
	data.custom2 = 0.0f;
	data.custom3 = 0.0f;
	data.distortion = 0;
	data.placeholder1 = 0.0f;
	data.placeholder2 = 0.0f;
	data.placeholder3 = 0.0f;

	sampler2Ds["texture0"] = 0;
}
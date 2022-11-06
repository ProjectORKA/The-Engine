
#include "Uniforms.hpp"

Int& Uniforms::width() {
	update = true;
	return data.width;
}
void Uniforms::reset()
{
	data.mMatrix = Matrix(1);
	data.vMatrix = Matrix(1);
	data.pMatrix = Matrix(1);
	data.worldOffset = Vec4(0, 0, 0, 0);
	data.cameraVector = Vec4(0, 0, 0, 0);
	data.cameraPosition = Vec4(0, 0, 0, 0);
	data.customColor = Vec4(1, 1, 1, 1);
	//screenDimensions = Vec4(1, 1, 0, 0);
	//data.sunDir = Vec4(0, 0, 1, 1);
	//data.time = 0.0f;
	data.custom1 = 0.0f;
	data.custom2 = 0.0f;
	data.custom3 = 0.0f;
	data.customInt1 = 0;
	//data.width = 1;
	//data.height = 1;
	data.instanced = 0;
	data.objectID = -1;
	data.materialID = -1;
	sampler2Ds["texture0"] = 0;
}
void Uniforms::create()
{
	reset();

	uniformBlockShaderCode = fileSystem.loadString("data/shaders/uniforms.glsl");

	apiGenBuffer(id);
	apiBindBuffer(GL_UNIFORM_BUFFER, id);
	apiBufferData(GL_UNIFORM_BUFFER, sizeof(GlobalUniformData), &data, GL_DYNAMIC_DRAW);
	//create binding
	apiBindBufferBase(GL_UNIFORM_BUFFER, 0, id);
	
}
void Uniforms::upload()
{
	apiBindBuffer(GL_UNIFORM_BUFFER, id);
	apiBufferData(GL_UNIFORM_BUFFER, sizeof(GlobalUniformData), &data, GL_DYNAMIC_DRAW);
}
Int& Uniforms::height() {
	update = true;
	return data.height;
}
Float& Uniforms::time() {
	update = true;
	return data.time;
}
void Uniforms::destroy()
{
	apiDeleteBuffer(id);
}
void Uniforms::resetMatrices() {
	worldOffset(Vec4(0));
	mMatrix(Matrix(1));
	vMatrix(Matrix(1));
	pMatrix(Matrix(1));
}
Vec4& Uniforms::sunDir() {
	update = true;
	return data.sunDir;
}
Int& Uniforms::objectID() {
	update = true;
	return data.objectID;
}
Float& Uniforms::custom1() {
	update = true;
	return data.custom1;
}
Int& Uniforms::instanced() {
	update = true;
	return data.instanced;
}
Int& Uniforms::customInt1() {
	update = true;
	return data.customInt1;
}
Vec4& Uniforms::cameraPos() {
	update = true;
	return data.cameraPosition;
}
Vec4& Uniforms::cameraVec() {
	update = true;
	return data.cameraVector;
}
Matrix& Uniforms::mMatrix() {
	update = true;
	return data.mMatrix;
}
Matrix& Uniforms::vMatrix() {
	update = true;
	return data.vMatrix;
}
Matrix& Uniforms::pMatrix() {
	update = true;
	return data.pMatrix;
}
Vec4& Uniforms::worldOffset() {
	update = true;
	return data.worldOffset;
}
Vec4& Uniforms::customColor() {
	update = true;
	return data.customColor;
}

Int& Uniforms::width(Int value) {
	if (value != data.width) {
		data.width = value;
		update = true;
	}
	return data.width;
}
Int& Uniforms::height(Int value) {
	if (value != data.height) {
		data.height = value;
		update = true;
	}
	return data.height;
}
Vec4& Uniforms::sunDir(Vec4 value) {
	if (value != data.sunDir) {
		data.sunDir = value;
		update = true;
	}
	return data.sunDir;
}
Float& Uniforms::time(Float value) {
	if (value != data.time) {
		data.time = value;
		update = true;
	}
	return data.time;
}
Int& Uniforms::objectID(UInt value) {
	if (value != data.objectID) {
		data.objectID = value;
		update = true;
	}
	return data.objectID;
}
Int& Uniforms::instanced(Int value) {
	if (value != data.instanced) {
		data.instanced = value;
		update = true;
	}
	return data.customInt1;
}
Int& Uniforms::customInt1(Int value) {
	if (value != data.customInt1) {
		data.customInt1 = value;
		update = true;
	}
	return data.customInt1;
}
Float& Uniforms::custom1(Float value) {
	if (value != data.custom1) {
		data.custom1 = value;
		update = true;
	}
	return data.custom1;
}
Vec4& Uniforms::cameraPos(Vec4 value) {
	if (value != data.cameraPosition) {
		data.cameraPosition = value;
		update = true;
	}
	return data.cameraPosition;
}
Vec4& Uniforms::cameraVec(Vec4 value) {
	if (value != data.cameraVector) {
		data.cameraVector = value;
		update = true;
	}
	return data.cameraVector;
}
Vec4& Uniforms::customColor(Vec4 value) {
	if (value != data.customColor) {
		data.customColor = value;
		update = true;
	}
	return data.customColor;
}
Vec4& Uniforms::worldOffset(Vec4 value) {
	if (value != data.worldOffset) {
		data.worldOffset = value;
		update = true;
	}
	return data.worldOffset;
}
Matrix& Uniforms::mMatrix(Matrix value) {
	if (value != data.mMatrix) {
		data.mMatrix = value;
		update = true;
	}
	return data.mMatrix;
}
Matrix& Uniforms::vMatrix(Matrix value) {
	if (value != data.vMatrix) {
		data.vMatrix = value;
		update = true;
	}
	return data.vMatrix;
}
Matrix& Uniforms::pMatrix(Matrix value) {
	if (value != data.pMatrix) {
		data.pMatrix = value;
		update = true;
	}
	return data.pMatrix;
}
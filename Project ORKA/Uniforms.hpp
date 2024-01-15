#pragma once

#include "Basics.hpp"
#include "GraphicsAPI.hpp"


struct GlobalUniformData
{
	// values have to be laid out in blocks of 16 bytes in memory
	// data has to match up with "uniforms.glsl"
	Matrix mMatrix           = Matrix(1);// 16 * 4
	Matrix vMatrix           = Matrix(1);// 16 * 4
	Matrix pMatrix           = Matrix(1);// 16 * 4
	Vec4   worldOffset       = Vec4(0, 0, 0, 0);// 16
	Vec4   cameraVector      = Vec4(0, 0, 0, 0);// 16
	Vec4   cameraPosition    = Vec4(0, 0, 0, 0);// 16
	Vec4   customColor       = Vec4(0, 0, 0, 1);// 16
	Vec4   sunDir            = Vec4(0, 0, 1, 1);// 16
	Float  time              = 0.0f;// 16
	Float  custom1           = 0.0f;
	Float  custom2           = 0.0f;
	Float  custom3           = 0.0f;
	Int    windowWidth       = 0;// 16
	Int    windowHeight      = 0;
	Int    framebufferWidth  = 0;
	Int    framebufferHeight = 0;
	Int    materialId        = 0;// 16
	Int    objectId          = 0;
	Int    instanced         = 0;
	Int    customInt1        = 0;
};

struct Uniforms
{
	void   reset();
	void   upload();
	void   destroy() const;
	void   resetMatrices();
	String getUniformShaderCode();
	void   create();

	[[nodiscard]] Float  getTime() const;
	[[nodiscard]] UInt   getOpenGLID() const;
	[[nodiscard]] Vec4   getSunDir() const;
	[[nodiscard]] Float  getCustom1() const;
	[[nodiscard]] Matrix getMMatrix() const;
	[[nodiscard]] Matrix getVMatrix() const;
	[[nodiscard]] Matrix getPMatrix() const;
	[[nodiscard]] Int    getObjectId() const;
	[[nodiscard]] Int    getInstanced() const;
	[[nodiscard]] Vec4   getCameraPos() const;
	[[nodiscard]] Vec4   getCameraVec() const;
	[[nodiscard]] Int    getCustomInt1() const;
	[[nodiscard]] Int    getWindowWidth() const;
	[[nodiscard]] Vec4   getCustomColor() const;
	[[nodiscard]] Vec4   getWorldOffset() const;
	[[nodiscard]] Int    getWindowHeight() const;
	[[nodiscard]] Int    getFramebufferWidth() const;
	[[nodiscard]] Int    getFramebufferHeight() const;

	void setTime(Float value);
	void setSunDir(Vec4 value);
	void setObjectId(UInt value);
	void setInstanced(Int value);
	void setCustom1(Float value);
	void setCustomInt1(Int value);
	void setCameraPos(Vec4 value);
	void setCameraVec(Vec4 value);
	void setWindowWidth(Int value);
	void setWindowHeight(Int value);
	void setCustomColor(Vec4 value);
	void setWorldOffset(Vec4 value);
	void setFramebufferWidth(Int value);
	void setFramebufferHeight(Int value);
	void setMMatrix(const Matrix& value);
	void setVMatrix(const Matrix& value);
	void setPMatrix(const Matrix& value);
private:
	GlobalUniformData   data;
	Map<Name, Index>    sampler2Ds;
	Map<Name, Index>    sampler2DMS;
	Bool                needsUpdate = true;
	OpenGLUniformBuffer openGlUniformBuffer;
	String              uniformBlockShaderCode;
};

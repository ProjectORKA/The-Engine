#pragma once

#include "Basics.hpp"
#include "GraphicsAPI.hpp"

struct Uniforms
{
	void   reset();
	void   resetMatrices();
	String getUniformShaderCode();
	String getComputeUniformShaderCode();
	void   create(OpenGlContext& openGlContext);
	void   upload(OpenGlContext& openGlContext);
	void   destroy(OpenGlContext& openGlContext);

	[[nodiscard]] Float  getTime() const;
	[[nodiscard]] UInt   getOpenGLID() const;
	[[nodiscard]] Vec4   getSunDir() const;
	[[nodiscard]] Float  getCustom1() const;
	[[nodiscard]] Float  getCustom2() const;
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
	void setCustom2(Float value);
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
	String              computeUniformBlockShaderCode;
};

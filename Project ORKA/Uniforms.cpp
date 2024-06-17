#include "Uniforms.hpp"
#include "FileSystem.hpp"
#include "GraphicsAPI.hpp"
#include "ResourceManager.hpp"

void Uniforms::reset()
{
	data.mMatrix        = Matrix(1);
	data.vMatrix        = Matrix(1);
	data.pMatrix        = Matrix(1);
	data.worldOffset    = Vec4(0, 0, 0, 0);
	data.cameraVector   = Vec4(0, 0, 0, 0);
	data.cameraPosition = Vec4(0, 0, 0, 0);
	data.customColor    = Vec4(1, 1, 1, 1);
	// screenDimensions = Vec4(1, 1, 0, 0);
	// data.sunDir = Vec4(0, 0, 1, 1);
	// data.time = 0.0f;
	data.custom1    = 0.0f;
	data.custom2    = 0.0f;
	data.custom3    = 0.0f;
	data.customInt1 = 0;
	// data.width = 1;
	// data.height = 1;
	data.instanced         = 0;
	data.objectId          = -1;
	data.materialId        = -1;
	sampler2Ds["texture0"] = 0;
	needsUpdate            = true;
}

void Uniforms::upload()
{
	if(needsUpdate)
	{
		openGlUniformBuffer.update(data);
		needsUpdate = false;
	}
}

void Uniforms::destroy() const
{
	openGlUniformBuffer.destroy();
}

void Uniforms::resetMatrices()
{
	setWorldOffset(Vec4(0));
	setMMatrix(Matrix(1));
	setVMatrix(Matrix(1));
	setPMatrix(Matrix(1));
	needsUpdate = true;
}

Float Uniforms::getTime() const
{
	return data.time;
}

Vec4 Uniforms::getSunDir() const
{
	return data.sunDir;
}

Int Uniforms::getObjectId() const
{
	return data.objectId;
}

UInt Uniforms::getOpenGLID() const
{
	return openGlUniformBuffer.getID();
}

Float Uniforms::getCustom1() const
{
	return data.custom1;
}

Int Uniforms::getInstanced() const
{
	return data.instanced;
}

Int Uniforms::getCustomInt1() const
{
	return data.customInt1;
}

Vec4 Uniforms::getCameraPos() const
{
	return data.cameraPosition;
}

Vec4 Uniforms::getCameraVec() const
{
	return data.cameraVector;
}

Matrix Uniforms::getMMatrix() const
{
	return data.mMatrix;
}

Matrix Uniforms::getVMatrix() const
{
	return data.vMatrix;
}

Matrix Uniforms::getPMatrix() const
{
	return data.pMatrix;
}

Int Uniforms::getWindowWidth() const
{
	return data.windowWidth;
}

Int Uniforms::getWindowHeight() const
{
	return data.windowHeight;
}

Vec4 Uniforms::getWorldOffset() const
{
	return data.worldOffset;
}

Vec4 Uniforms::getCustomColor() const
{
	return data.customColor;
}

String Uniforms::getUniformShaderCode()
{
	return uniformBlockShaderCode;
}

Int Uniforms::getFramebufferWidth() const
{
	return data.framebufferWidth;
}

void Uniforms::setTime(const Float value)
{
	if(value != data.time)
	{
		data.time   = value;
		needsUpdate = true;
	}
}

Int Uniforms::getFramebufferHeight() const
{
	return data.framebufferHeight;
}

void Uniforms::setSunDir(const Vec4 value)
{
	if(value != data.sunDir)
	{
		data.sunDir = value;
		needsUpdate = true;
	}
}

void Uniforms::setObjectId(const UInt value)
{
	if(value != data.objectId)
	{
		data.objectId = value;
		needsUpdate   = true;
	}
}

void Uniforms::setInstanced(const Int value)
{
	if(value != data.instanced)
	{
		data.instanced = value;
		needsUpdate    = true;
	}
}

void Uniforms::setCustom1(const Float value)
{
	if(value != data.custom1)
	{
		data.custom1 = value;
		needsUpdate  = true;
	}
}

void Uniforms::setCustomInt1(const Int value)
{
	if(value != data.customInt1)
	{
		data.customInt1 = value;
		needsUpdate     = true;
	}
}

void Uniforms::setCameraPos(const Vec4 value)
{
	if(value != data.cameraPosition)
	{
		data.cameraPosition = value;
		needsUpdate         = true;
	}
}

void Uniforms::setCameraVec(const Vec4 value)
{
	if(value != data.cameraVector)
	{
		data.cameraVector = value;
		needsUpdate       = true;
	}
}

void Uniforms::setMMatrix(const Matrix& value)
{
	if(value != data.mMatrix)
	{
		data.mMatrix = value;
		needsUpdate  = true;
	}
}

void Uniforms::setVMatrix(const Matrix& value)
{
	if(value != data.vMatrix)
	{
		data.vMatrix = value;
		needsUpdate  = true;
	}
}

void Uniforms::setPMatrix(const Matrix& value)
{
	if(value != data.pMatrix)
	{
		data.pMatrix = value;
		needsUpdate  = true;
	}
}

void Uniforms::setWindowWidth(const Int value)
{
	if(value != data.windowWidth)
	{
		data.windowWidth = value;
		needsUpdate      = true;
	}
}

void Uniforms::setCustomColor(const Vec4 value)
{
	if(value != data.customColor)
	{
		data.customColor = value;
		needsUpdate      = true;
	}
}

void Uniforms::setWorldOffset(const Vec4 value)
{
	if(value != data.worldOffset)
	{
		data.worldOffset = value;
		needsUpdate      = true;
	}
}

void Uniforms::setWindowHeight(const Int value)
{
	if(value != data.windowHeight)
	{
		data.windowHeight = value;
		needsUpdate       = true;
	}
}

void Uniforms::setFramebufferWidth(const Int value)
{
	if(value != data.framebufferWidth)
	{
		data.framebufferWidth = value;
		needsUpdate           = true;
	}
}

void Uniforms::setFramebufferHeight(const Int value)
{
	if(value != data.framebufferHeight)
	{
		data.framebufferHeight = value;
		needsUpdate            = true;
	}
}

void Uniforms::create()
{
	reset();
	const Path uniformShaderCodePath = resourceManager.getOrkaDataPath().append("shaders").append("uniforms.glsl");
	uniformBlockShaderCode           = loadString(uniformShaderCodePath);
	openGlUniformBuffer.create(data, 0);
	needsUpdate = true;
}

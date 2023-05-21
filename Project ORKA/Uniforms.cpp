#include "Uniforms.hpp"
#include "GraphicsAPI.hpp"

Int& Uniforms::windowWidth()
{
	update = true;
	return data.windowWidth;
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
	data.objectId = -1;
	data.materialId = -1;
	sampler2Ds["texture0"] = 0;
}

void Uniforms::create(const ResourceManager& resourceManager)
{
	reset();
	const Path uniformShaderCodePath = Path(resourceManager.getOrkaDataLocation()).append("shaders").append("uniforms.glsl");
	uniformBlockShaderCode = loadString(uniformShaderCodePath);
	apiGenBuffer(id);
	apiBindBuffer(GL_UNIFORM_BUFFER, id);
	apiBufferData(GL_UNIFORM_BUFFER, sizeof(GlobalUniformData), &data, MeshDrawMode::DynamicMode);
	//create binding
	apiBindBufferBase(GL_UNIFORM_BUFFER, 0, id);
}

void Uniforms::upload() const
{
	apiBindBuffer(GL_UNIFORM_BUFFER, id);
	apiBufferData(GL_UNIFORM_BUFFER, sizeof(GlobalUniformData), &data, MeshDrawMode::DynamicMode);
}

Int& Uniforms::windowHeight()
{
	update = true;
	return data.windowHeight;
}

Int& Uniforms::framebufferHeight()
{
	update = true;
	return data.framebufferHeight;
}

Int& Uniforms::framebufferWidth()
{
	update = true;
	return data.framebufferWidth;
}

Float& Uniforms::time()
{
	update = true;
	return data.time;
}

void Uniforms::destroy() const
{
	apiDeleteBuffer(id);
}

void Uniforms::resetMatrices()
{
	worldOffset(Vec4(0));
	mMatrix(Matrix(1));
	vMatrix(Matrix(1));
	pMatrix(Matrix(1));
}

Vec4& Uniforms::sunDir()
{
	update = true;
	return data.sunDir;
}

Int& Uniforms::objectId()
{
	update = true;
	return data.objectId;
}

Float& Uniforms::custom1()
{
	update = true;
	return data.custom1;
}

Int& Uniforms::instanced()
{
	update = true;
	return data.instanced;
}

Int& Uniforms::customInt1()
{
	update = true;
	return data.customInt1;
}

Vec4& Uniforms::cameraPos()
{
	update = true;
	return data.cameraPosition;
}

Vec4& Uniforms::cameraVec()
{
	update = true;
	return data.cameraVector;
}

Matrix& Uniforms::mMatrix()
{
	update = true;
	return data.mMatrix;
}

Matrix& Uniforms::vMatrix()
{
	update = true;
	return data.vMatrix;
}

Matrix& Uniforms::pMatrix()
{
	update = true;
	return data.pMatrix;
}

Vec4& Uniforms::worldOffset()
{
	update = true;
	return data.worldOffset;
}

Vec4& Uniforms::customColor()
{
	update = true;
	return data.customColor;
}

Int& Uniforms::windowWidth(const Int value)
{
	if(value != data.windowWidth)
	{
		data.windowWidth = value;
		update = true;
	}
	return data.windowWidth;
}

Int& Uniforms::windowHeight(const Int value)
{
	if(value != data.windowHeight)
	{
		data.windowHeight = value;
		update = true;
	}
	return data.windowHeight;
}

Int& Uniforms::framebufferWidth(const Int value)
{
	if(value != data.framebufferWidth)
	{
		data.framebufferWidth = value;
		update = true;
	}
	return data.framebufferWidth;
}

Int& Uniforms::framebufferHeight(const Int value)
{
	if(value != data.framebufferHeight)
	{
		data.framebufferHeight = value;
		update = true;
	}
	return data.framebufferHeight;
}

Vec4& Uniforms::sunDir(const Vec4 value)
{
	if(value != data.sunDir)
	{
		data.sunDir = value;
		update = true;
	}
	return data.sunDir;
}

Float& Uniforms::time(const Float value)
{
	if(value != data.time)
	{
		data.time = value;
		update = true;
	}
	return data.time;
}

Int& Uniforms::objectId(const UInt value)
{
	if(value != data.objectId)
	{
		data.objectId = value;
		update = true;
	}
	return data.objectId;
}

Int& Uniforms::instanced(const Int value)
{
	if(value != data.instanced)
	{
		data.instanced = value;
		update = true;
	}
	return data.customInt1;
}

Int& Uniforms::customInt1(const Int value)
{
	if(value != data.customInt1)
	{
		data.customInt1 = value;
		update = true;
	}
	return data.customInt1;
}

Float& Uniforms::custom1(const Float value)
{
	if(value != data.custom1)
	{
		data.custom1 = value;
		update = true;
	}
	return data.custom1;
}

Vec4& Uniforms::cameraPos(const Vec4 value)
{
	if(value != data.cameraPosition)
	{
		data.cameraPosition = value;
		update = true;
	}
	return data.cameraPosition;
}

Vec4& Uniforms::cameraVec(const Vec4 value)
{
	if(value != data.cameraVector)
	{
		data.cameraVector = value;
		update = true;
	}
	return data.cameraVector;
}

Vec4& Uniforms::customColor(const Vec4 value)
{
	if(value != data.customColor)
	{
		data.customColor = value;
		update = true;
	}
	return data.customColor;
}

Vec4& Uniforms::worldOffset(const Vec4 value)
{
	if(value != data.worldOffset)
	{
		data.worldOffset = value;
		update = true;
	}
	return data.worldOffset;
}

Matrix& Uniforms::mMatrix(const Matrix& value)
{
	if(value != data.mMatrix)
	{
		data.mMatrix = value;
		update = true;
	}
	return data.mMatrix;
}

Matrix& Uniforms::vMatrix(const Matrix& value)
{
	if(value != data.vMatrix)
	{
		data.vMatrix = value;
		update = true;
	}
	return data.vMatrix;
}

Matrix& Uniforms::pMatrix(const Matrix& value)
{
	if(value != data.pMatrix)
	{
		data.pMatrix = value;
		update = true;
	}
	return data.pMatrix;
}
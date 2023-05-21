#include "Shader.hpp"
#include "GraphicsAPI.hpp"
#include "FileSystem.hpp"

void Shader::destroy()
{
	isLoaded = false;
	apiDeleteShader(shaderId);
}

void Shader::create(const Path& path, const String& uniformBlock)
{
	ShaderType shaderType = ShaderType::Fragment;
	const String extension = path.extension().string();

	if(extension == ".vert") shaderType = ShaderType::Vertex;
	else if(extension == ".frag") shaderType = ShaderType::Fragment;
		//[TODO] add all
	else logError("Shader type not supported!");

	String shaderCode = uniformBlock;
	shaderCode.append(loadString(path));

	loadShaderCode(shaderType, shaderCode);
}

void Shader::loadShaderCode(const ShaderType shaderType, const String& shaderCode)
{
	shaderId = apiCreateShader(shaderType);

	const char* SourcePointer = shaderCode.c_str();
	apiShaderSource(shaderId, 1, &SourcePointer, nullptr);
	apiCompileShader(shaderId);

	Int result = apiGetShaderIntegerValue(shaderId, GL_COMPILE_STATUS);
	const Int infoLogLength = apiGetShaderIntegerValue(shaderId, GL_INFO_LOG_LENGTH);

	if(infoLogLength > 0)
	{
		logError(apiGetShaderInfoLog(shaderId, infoLogLength));
		isLoaded = false;
	}
	else
	{
		isLoaded = true;
	}
}
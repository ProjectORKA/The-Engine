#include "Shader.hpp"
#include "Util.hpp"

void Shader::unload() {
	isLoaded = false;
	apiDeleteShader(shaderID);
}
void Shader::load(ShaderType shaderType, Name name)
{
	switch (shaderType) {
	case ShaderType::vertex: load(shaderType, String("Data/shaders/").append(name.data).append(".vert")); break;
	case ShaderType::compute: load(shaderType, String("Data/shaders/").append(name.data).append(".comp")); break;
	case ShaderType::geometry: load(shaderType, String("Data/shaders/").append(name.data).append(".geom")); break;
	case ShaderType::fragment: load(shaderType, String("Data/shaders/").append(name.data).append(".frag")); break;
	case ShaderType::tessellationControl: load(shaderType, String("Data/shaders/").append(name.data).append(".tesc")); break;
	case ShaderType::tessellationEvaluation: load(shaderType, String("Data/shaders/").append(name.data).append(".tese")); break;
		//etc
	default: logError("Unknown shaderType!");
	}
}
void Shader::load(ShaderType shaderType, Path path) {
	String shaderCode = loadString(path);
	loadShaderCode(shaderType, shaderCode);
}
void Shader::loadShaderCode(ShaderType shaderType, String shaderCode)
{
	shaderID = apiCreateShader(enumClassAsInt(shaderType));

	char const* SourcePointer = shaderCode.c_str();
	apiShaderSource(shaderID, 1, &SourcePointer, NULL);
	apiCompileShader(shaderID);

	Int result = apiGetShaderiv(shaderID, GL_COMPILE_STATUS);
	Int infoLogLength = apiGetShaderiv(shaderID, GL_INFO_LOG_LENGTH);

	if (infoLogLength > 0) {
		logError(apiGetShaderInfoLog(shaderID,infoLogLength));
		isLoaded = false;
	}
	else {
		isLoaded = true;
	}
}
#include "Shader.hpp"
#include "Util.hpp"

void Shader::destroy() {
	isLoaded = false;
	apiDeleteShader(shaderID);
}
void Shader::create(ShaderType shaderType, Name name, String uniformBlock)
{
	logDebug(String("Loading ShaderProgram (").append(String(name.data)).append("):"));
	switch (shaderType) {
	case ShaderType::vertex: create(shaderType, String("Data/shaders/").append(name.data).append(".vert"), uniformBlock); break;
	case ShaderType::compute: create(shaderType, String("Data/shaders/").append(name.data).append(".comp"), uniformBlock); break;
	case ShaderType::geometry: create(shaderType, String("Data/shaders/").append(name.data).append(".geom"), uniformBlock); break;
	case ShaderType::fragment: create(shaderType, String("Data/shaders/").append(name.data).append(".frag"), uniformBlock); break;
	case ShaderType::tessellationControl: create(shaderType, String("Data/shaders/").append(name.data).append(".tesc"), uniformBlock); break;
	case ShaderType::tessellationEvaluation: create(shaderType, String("Data/shaders/").append(name.data).append(".tese"), uniformBlock); break;
		//etc
	default: logError("Unknown shaderType!");
	}
}
void Shader::create(ShaderType shaderType, Path path, String uniformBlock) {
	String shaderCode = uniformBlock;
	shaderCode.append(loadString(path));
	loadShaderCode(shaderType, shaderCode);
}
void Shader::loadShaderCode(ShaderType shaderType, String shaderCode)
{
	//logDebug(String("\n\Loading Shader:--------------------------------------------------------").append(shaderCode).append("\n\---------------------------------------------------------------------- "));

	shaderID = apiCreateShader(enumClassAsInt(shaderType));

	char const* SourcePointer = shaderCode.c_str();
	apiShaderSource(shaderID, 1, &SourcePointer, NULL);
	apiCompileShader(shaderID);

	Int result = apiGetShaderiv(shaderID, GL_COMPILE_STATUS);
	Int infoLogLength = apiGetShaderiv(shaderID, GL_INFO_LOG_LENGTH);

	if (infoLogLength > 0) {
		logError(apiGetShaderInfoLog(shaderID, infoLogLength));
		isLoaded = false;
	}
	else {
		isLoaded = true;
	}
}
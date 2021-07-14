#include "Shader.hpp"
#include "Util.hpp"
#include "FileSystem.hpp"

void Shader::unload() {
	isValid = false;
	glDeleteShader(shaderID);
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
	String shaderCode;
	loadString(shaderCode, path);
	loadShaderCode(shaderType, shaderCode);
}
void Shader::loadShaderCode(ShaderType shaderType, String shaderCode)
{
	shaderID = glCreateShader(enumClassAsInt(shaderType));

	char const* SourcePointer = shaderCode.c_str();
	glShaderSource(shaderID, 1, &SourcePointer, NULL);
	glCompileShader(shaderID);

	GLint result = GL_FALSE;
	GLint infoLogLength = 0;

	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		Vector<char> shaderErrorMessage(infoLogLength++);
		glGetShaderInfoLog(shaderID, infoLogLength, NULL, &shaderErrorMessage[0]);
		logError(&shaderErrorMessage[0]);
		isValid = false;
	}
	else {
		isValid = true;
	}
}
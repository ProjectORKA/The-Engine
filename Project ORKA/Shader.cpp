#include "Shader.hpp"

void Shader::destroy() {
	isLoaded = false;
	apiDeleteShader(shaderID);
}
void Shader::create(Path path, String uniformBlock) {
	Int shaderType;
	String extension = path.extension().string();
	
	if (extension == ".vert") shaderType = ShaderType::vertex;
	else if (extension == ".frag") shaderType = ShaderType::fragment;
	//[TODO] add all
	else logError("Shadertype not supported!");

	String shaderCode = uniformBlock;
	shaderCode.append(fileSystem.loadString(path));

	loadShaderCode(shaderType, shaderCode);
}
void Shader::loadShaderCode(Int shaderType, String shaderCode)
{

	shaderID = apiCreateShader(shaderType);

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
#include "Shader.hpp"

#include "GraphicsAPI.hpp"

void Shader::destroy() {
	isLoaded = false;
	apiDeleteShader(shaderID);
}

void Shader::create(const Path& path, const String& uniformBlock) {
	Int shaderType = -1;
	const String extension = path.extension().string();

	if (extension == ".vert") shaderType = vertex;
	else if (extension == ".frag") shaderType = fragment;
		//[TODO] add all
	else logError("Shadertype not supported!");

	String shaderCode = uniformBlock;
	shaderCode.append(loadString(path));

	loadShaderCode(shaderType, shaderCode);
}

void Shader::loadShaderCode(const Int shaderType, const String& shaderCode) {
	shaderID = apiCreateShader(shaderType);

	const char* SourcePointer = shaderCode.c_str();
	apiShaderSource(shaderID, 1, &SourcePointer, nullptr);
	apiCompileShader(shaderID);

	Int result = apiGetShaderiv(shaderID, GL_COMPILE_STATUS);
	const Int infoLogLength = apiGetShaderiv(shaderID, GL_INFO_LOG_LENGTH);

	if (infoLogLength > 0) {
		logError(apiGetShaderInfoLog(shaderID, infoLogLength));
		isLoaded = false;
	}
	else { isLoaded = true; }
}

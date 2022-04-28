
#include "ShaderProgram.hpp"
#include "ResourceManager.hpp"

void ShaderProgram::select() {
	if (isLoaded) {
		apiUseProgram(programID);
	}
	else {
		logError("ShaderProgram not loaded!");
	}
};
void ShaderProgram::destroy()
{
	logDebug("Unloading Shader!");
	if (isLoaded) {
		apiDeleteProgram(programID);
	}
	else {
		logDebug("ShaderProgram already unloaded!");
	}
	logDebug("Unloaded Shader!");
}
void ShaderProgram::create(Name name, Uniforms& uniforms) {
	
	auto vertIt = resourceManager->vertexShaderResources.find(name);
	Shader vertexShader;
	if (vertIt != resourceManager->vertexShaderResources.end()) {
		vertexShader.create(vertIt->second, uniforms.uniformBlockShaderCode);
	}
	else {
		logError("Vertex shader could not be found!");
	}

	auto fragIt = resourceManager->fragmentShaderResources.find(name);
	Shader fragmentShader;
	if (fragIt != resourceManager->fragmentShaderResources.end()) {
		fragmentShader.create(fragIt->second, uniforms.uniformBlockShaderCode);
	}
	else {
		logError("Fragment shader could not be found!");
	}

	create(vertexShader, fragmentShader, uniforms);
}
void ShaderProgram::create(Shader& vertexShader, Shader& fragmentShader, Uniforms& uniforms) {

	logDebug("Creating Shader!");

	if (isLoaded) {
		logError("ShaderProgram already loaded!");
	}
	else {
		programID = apiCreateProgram();
		apiAttachShader(programID, vertexShader.shaderID);
		apiAttachShader(programID, fragmentShader.shaderID);
		apiLinkProgram(programID);

		Int result = apiGetProgramIntegerValue(programID, GL_LINK_STATUS);
		Int infoLogLength = apiGetProgramIntegerValue(programID, GL_INFO_LOG_LENGTH);

		if (infoLogLength > 0) {
			logError(apiGetProgramInfoLog(programID, infoLogLength));
		}

		apiDetachShader(programID, vertexShader.shaderID);
		apiDetachShader(programID, fragmentShader.shaderID);

		apiUseProgram(programID);

		apiUniform1i(apiGetUniformLocation(programID, "texture0"), 0);
		apiUniform1i(apiGetUniformLocation(programID, "texture1"), 1);
		apiUniform1i(apiGetUniformLocation(programID, "texture2"), 2);
		apiUniform1i(apiGetUniformLocation(programID, "texture3"), 3);
		apiUniform1i(apiGetUniformLocation(programID, "texture4"), 4);
		apiUniform1i(apiGetUniformLocation(programID, "texture5"), 5);
		apiUniform1i(apiGetUniformLocation(programID, "texture6"), 6);
		apiUniform1i(apiGetUniformLocation(programID, "texture7"), 7);
		apiUniform1i(apiGetUniformLocation(programID, "texture8"), 8);
		apiUniform1i(apiGetUniformLocation(programID, "texture9"), 9);
		apiUniform1i(apiGetUniformLocation(programID, "texture10"), 10);
		apiUniform1i(apiGetUniformLocation(programID, "texture11"), 11);
		apiUniform1i(apiGetUniformLocation(programID, "texture12"), 12);
		apiUniform1i(apiGetUniformLocation(programID, "texture13"), 13);
		apiUniform1i(apiGetUniformLocation(programID, "texture14"), 14);
		apiUniform1i(apiGetUniformLocation(programID, "texture15"), 15);

		//create binding to uniform buffer
		apiUniformBlockBinding(programID, apiGetUniformBlockIndex(programID, "GlobalUniforms"), 0);

		isLoaded = true;
	}

	logDebug("Created Shader!");
}

#include "ShaderProgram.hpp"

void ShaderProgram::select() {
	if (isValid) {
		apiUseProgram(programID);
	}
	else {
		logError("ShaderProgram not loaded!");
	}
};
void ShaderProgram::destroy()
{
	logDebug("Unloading Shader!");
	if (isValid) {
		apiDeleteProgram(programID);
	}
	else {
		logDebug("ShaderProgram already unloaded!");
	}
	logDebug("Unloaded Shader!");
}
void ShaderProgram::create(Name name, Uniforms& uniforms) {
	Shader vertexShader;
	Shader fragmentShader;
	vertexShader.load(ShaderType::vertex, name);
	fragmentShader.load(ShaderType::fragment, name);

	create(vertexShader, fragmentShader, uniforms);
}
void ShaderProgram::create(Shader& vertexShader, Shader& fragmentShader, Uniforms& uniforms) {

	logDebug("Creating Shader!");

	//set pointer to allow the shader to update uniforms by itself
	//uniformsPtr = &uniforms;

	if (isValid) {
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

		isValid = true;
	}

	logDebug("Created Shader!");
}
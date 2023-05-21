#include "ShaderProgram.hpp"

void ShaderProgram::select() const
{
	if(isLoaded)
	{
		apiUseProgram(programId);
	}
	else
	{
		logError("ShaderProgram not loaded!");
	}
};

void ShaderProgram::destroy() const
{
	logDebug("Unloading Shader!");
	if(isLoaded)
	{
		apiDeleteProgram(programId);
	}
	else
	{
		logDebug("ShaderProgram already unloaded!");
	}
	logDebug("Unloaded Shader!");
}

void ShaderProgram::create(ResourceManager& resourceManager, const Name& name, Uniforms& uniforms)
{
	Shader vertexShader;
	vertexShader.create(resourceManager.getVertexShaderResourcePath(name), uniforms.uniformBlockShaderCode);

	Shader fragmentShader;
	fragmentShader.create(resourceManager.getFragmentShaderResourcePath(name), uniforms.uniformBlockShaderCode);

	create(vertexShader, fragmentShader, uniforms);
}

void ShaderProgram::create(const Shader& vertexShader, const Shader& fragmentShader, Uniforms& uniforms)
{
	logDebug("Creating Shader!");

	if(isLoaded)
	{
		logError("ShaderProgram already loaded!");
	}
	else
	{
		programId = apiCreateProgram();
		apiAttachShader(programId, vertexShader.shaderId);
		apiAttachShader(programId, fragmentShader.shaderId);
		apiLinkProgram(programId);
		const Int infoLogLength = apiGetProgramIntegerValue(programId, GL_INFO_LOG_LENGTH);
		if(infoLogLength > 1)
		{
			logError(apiGetProgramInfoLog(programId, infoLogLength));
		}
		apiDetachShader(programId, vertexShader.shaderId);
		apiDetachShader(programId, fragmentShader.shaderId);
		apiUseProgram(programId);
		apiUniformInt1(apiGetUniformLocation(programId, "texture0"), 0);
		apiUniformInt1(apiGetUniformLocation(programId, "texture1"), 1);
		apiUniformInt1(apiGetUniformLocation(programId, "texture2"), 2);
		apiUniformInt1(apiGetUniformLocation(programId, "texture3"), 3);
		apiUniformInt1(apiGetUniformLocation(programId, "texture4"), 4);
		apiUniformInt1(apiGetUniformLocation(programId, "texture5"), 5);
		apiUniformInt1(apiGetUniformLocation(programId, "texture6"), 6);
		apiUniformInt1(apiGetUniformLocation(programId, "texture7"), 7);
		apiUniformInt1(apiGetUniformLocation(programId, "texture8"), 8);
		apiUniformInt1(apiGetUniformLocation(programId, "texture9"), 9);
		apiUniformInt1(apiGetUniformLocation(programId, "texture10"), 10);
		apiUniformInt1(apiGetUniformLocation(programId, "texture11"), 11);
		apiUniformInt1(apiGetUniformLocation(programId, "texture12"), 12);
		apiUniformInt1(apiGetUniformLocation(programId, "texture13"), 13);
		apiUniformInt1(apiGetUniformLocation(programId, "texture14"), 14);
		apiUniformInt1(apiGetUniformLocation(programId, "texture15"), 15);

		//create binding to uniform buffer
		apiUniformBlockBinding(programId, apiGetUniformBlockIndex(programId, "GlobalUniforms"), 0);

		isLoaded = true;
	}
	logDebug("Created Shader!");
}
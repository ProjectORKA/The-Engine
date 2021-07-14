
#include "ShaderProgram.hpp"

void ShaderProgram::select() {
	if (isValid) {
		glUseProgram(programID);
	}
	else {
		logError("ShaderProgram not loaded!");
	}
};
void ShaderProgram::unload()
{
	logDebug("Unloading Shader!");
	if (isValid) {
		glDeleteProgram(programID);
	}
	else {
		logDebug("ShaderProgram already unloaded!");
	}
	logDebug("Unloaded Shader!");
}
void ShaderProgram::load(Name name, Uniforms& uniforms) {
	Shader vertexShader;
	Shader fragmentShader;
	vertexShader.load(ShaderType::vertex, name);
	fragmentShader.load(ShaderType::fragment, name);

	load(vertexShader, fragmentShader, uniforms);
}
void ShaderProgram::load(Shader& vertexShader, Shader& fragmentShader, Uniforms& uniforms) {

	logDebug("Creating Shader!");

	//set pointer to allow the shader to update uniforms by itself
	uniformsPtr = &uniforms;

	if (isValid) {
		logError("ShaderProgram already loaded!");
	}
	else {
		programID = glCreateProgram();
		glAttachShader(programID, vertexShader.shaderID);
		glAttachShader(programID, fragmentShader.shaderID);
		glLinkProgram(programID);

		GLint Result = GL_FALSE;
		GLint InfoLogLength = 0;
		glGetProgramiv(programID, GL_LINK_STATUS, &Result);
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> ProgramErrorMessage(InfoLogLength++);
			glGetProgramInfoLog(programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			printf("%s\n", &ProgramErrorMessage[0]);
		}

		glDetachShader(programID, vertexShader.shaderID);
		glDetachShader(programID, fragmentShader.shaderID);

		glUseProgram(programID);

		glUniform1i(glGetUniformLocation(programID, "texture0"), 0);
		glUniform1i(glGetUniformLocation(programID, "texture1"), 1);
		glUniform1i(glGetUniformLocation(programID, "texture2"), 2);
		glUniform1i(glGetUniformLocation(programID, "texture3"), 3);
		glUniform1i(glGetUniformLocation(programID, "texture4"), 4);
		glUniform1i(glGetUniformLocation(programID, "texture5"), 5);
		glUniform1i(glGetUniformLocation(programID, "texture6"), 6);
		glUniform1i(glGetUniformLocation(programID, "texture7"), 7);
		glUniform1i(glGetUniformLocation(programID, "texture8"), 8);
		glUniform1i(glGetUniformLocation(programID, "texture9"), 9);
		glUniform1i(glGetUniformLocation(programID, "texture10"), 10);
		glUniform1i(glGetUniformLocation(programID, "texture11"), 11);
		glUniform1i(glGetUniformLocation(programID, "texture12"), 12);
		glUniform1i(glGetUniformLocation(programID, "texture13"), 13);
		glUniform1i(glGetUniformLocation(programID, "texture14"), 14);
		glUniform1i(glGetUniformLocation(programID, "texture15"), 15);

		//create binding to uniform buffer
		glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "GlobalUniforms"), 0);

		isValid = true;
	}

	logDebug("Created Shader!");
}
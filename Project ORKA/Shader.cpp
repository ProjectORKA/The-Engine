
#include "Shader.hpp"

void Shader::unload() {
	isValid = false;
	glDeleteShader(shaderID);
}
void Shader::loadFromFile(Index shaderType, Path path) {
	String shaderCode;
	loadString(shaderCode, path);
	load(shaderType, shaderCode);
}
void Shader::load(Index shaderType, String shaderCode)
{
	shaderID = glCreateShader(shaderType);

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

void ShaderProgram::use() {
	if (isValid) {
		glUseProgram(programID);
		updateUniforms();
	}
	else {
		logError("ShaderProgram not loaded!");
	}
};
void ShaderProgram::unload()
{
	if (isValid) {
		glDeleteProgram(programID);
	}
	else {
		logDebug("ShaderProgram already unloaded!");
	}
}
void ShaderProgram::updateUniforms()
{
	for (Uniform u : usedUniforms) {
		switch (u.type) {
		case GL_BOOL: glUniform1i(u.uniformID, uniformsPtr->bools[u.name]);
			break;
		case GL_INT: glUniform1i(u.uniformID, uniformsPtr->ints[u.name]);
			break;
		case GL_FLOAT: glUniform1f(u.uniformID, uniformsPtr->floats[u.name]);
			break;
		case GL_FLOAT_VEC3:
			Vec3 tmpVec3 = uniformsPtr->vec3s[u.name];
			glUniform3f(u.uniformID, tmpVec3.x, tmpVec3.y, tmpVec3.z);
			break;
		case GL_FLOAT_VEC4:
			Vec4 tmpVec4 = uniformsPtr->vec4s[u.name];
			glUniform4f(u.uniformID, tmpVec4.x, tmpVec4.y, tmpVec4.z, tmpVec4.w);
			break;
		case GL_FLOAT_MAT4: glUniformMatrix4fv(u.uniformID, 1, GL_FALSE, glm::value_ptr(uniformsPtr->matrices[u.name]));
			break;
		case GL_SAMPLER_2D: glUniform1i(u.uniformID, uniformsPtr->sampler2Ds["texture0"]);
			break;

		default : logError("GLSL Uniform type not supported!");
		}
	}
}
void ShaderProgram::create(Shader& vertexShader, Shader& fragmentShader, Uniforms& uniforms) {
	uniformsPtr = &uniforms;

	if (isValid) {
		logError("ShaderProgram already loaded!");
	} else {
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

		//check for all uniforms
		GLint id;
		GLint count;
		GLint size; 
		GLenum type; 
		const GLsizei bufSize = NAME_SIZE;
		GLchar name[bufSize];
		GLsizei length;

		glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &count);
		Uniform tmp;
		for (id = 0; id < count; id++)
		{
			glGetActiveUniform(programID, (GLuint)id, bufSize, &length, &size, &type, name);

			tmp.uniformID = id;
			tmp.type = type;
			tmp.name = name;

			usedUniforms.push_back(tmp);
		}

		isValid = true;
	}
}

#include "Program.hpp"

void loadShader(Shader& shader, GLuint shaderType, const char* shaderPath){
	shader.shaderID = glCreateShader(shaderType);
	std::string ShaderCode;
	std::ifstream ShaderStream(shaderPath, std::ios::in);

	try {
		if (ShaderStream.is_open()) {
			std::stringstream sstr;
			sstr << ShaderStream.rdbuf();
			ShaderCode = sstr.str();
			ShaderStream.close();
		}
		else {
			throw std::exception{ "Could not load shader!" };
		}
	}
	catch (std::exception error) {
		std::cout << "Error: " << error.what() << std::endl;
		std::getchar();
		exit(EXIT_FAILURE);
	};

	char const* SourcePointer = ShaderCode.c_str();
	glShaderSource(shader.shaderID, 1, &SourcePointer, NULL);
	glCompileShader(shader.shaderID);

	GLint Result = GL_FALSE;
	GLint InfoLogLength = 0;

	glGetShaderiv(shader.shaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shader.shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ShaderErrorMessage(InfoLogLength++);
		glGetShaderInfoLog(shader.shaderID, InfoLogLength, NULL, &ShaderErrorMessage[0]);
		printf("%s\n", &ShaderErrorMessage[0]);
	}
}

void unloadShader(Shader& shader) {
	glDeleteShader(shader.shaderID);
}

void loadShaderProgram(ShaderProgram& shaderProgram, Shader & vertexShader, Shader & fragmentShader){
	// Link the program
	shaderProgram.programID = glCreateProgram();
	glAttachShader(shaderProgram.programID, vertexShader.shaderID);
	glAttachShader(shaderProgram.programID, fragmentShader.shaderID);
	glLinkProgram(shaderProgram.programID);


	// Check the program
	GLint Result = GL_FALSE;
	GLint InfoLogLength = 0;
	glGetProgramiv(shaderProgram.programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(shaderProgram.programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength++);
		glGetProgramInfoLog(shaderProgram.programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(shaderProgram.programID, vertexShader.shaderID);
	glDetachShader(shaderProgram.programID, fragmentShader.shaderID);

	glUseProgram(shaderProgram.programID);

	//set up matrix ID
	shaderProgram.timeID = glGetUniformLocation(shaderProgram.programID, "time");
	shaderProgram.chunkOffsetVectorID = glGetUniformLocation(shaderProgram.programID, "chunkOffsetVector");
	shaderProgram.mMatrixID = glGetUniformLocation(shaderProgram.programID, "mMatrix");
	shaderProgram.vpMatrixID = glGetUniformLocation(shaderProgram.programID, "vpMatrix");
	shaderProgram.worldOffsetID = glGetUniformLocation(shaderProgram.programID, "worldOffset");
	
	shaderProgram.loaded = true;
}

void unloadShaderProgram(ShaderProgram& shaderProgram) {
	glDeleteProgram(shaderProgram.programID);
}

void useShaderProgram(ShaderProgram & shaderProgram) {
	if (shaderProgram.loaded) {
		glUseProgram(shaderProgram.programID);
	}
}
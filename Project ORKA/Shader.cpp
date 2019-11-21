
#include "Program.hpp"

void loadShader(ShaderProgram & shaderProgram, const char * vertexPath, const char * fragmentPath) {
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertexPath, std::ios::in);
	try {
		if (VertexShaderStream.is_open()) {
			std::stringstream sstr;
			sstr << VertexShaderStream.rdbuf();
			VertexShaderCode = sstr.str();
			VertexShaderStream.close();
		}
		else {
			throw std::exception{ "Could not load vertex shader!" };
		}
	}
	catch (std::exception error) {
		std::cout << "Error: " << error.what() << std::endl;
		std::getchar();
		exit(EXIT_FAILURE);
	};

	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragmentPath, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}
	else {
		throw std::runtime_error{ "Could not load fragment shader!" };
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	shaderProgram.programID = glCreateProgram();
	glAttachShader(shaderProgram.programID, VertexShaderID);
	glAttachShader(shaderProgram.programID, FragmentShaderID);
	glLinkProgram(shaderProgram.programID);

	// Check the program
	glGetProgramiv(shaderProgram.programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(shaderProgram.programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(shaderProgram.programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(shaderProgram.programID, VertexShaderID);
	glDetachShader(shaderProgram.programID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	glUseProgram(shaderProgram.programID);

	//set up matrix IDs
	//shaderProgram.modelMatrixID = glGetUniformLocation(shaderProgram.programID, "modelMatrix");
	//shaderProgram.viewMatrixID = glGetUniformLocation(shaderProgram.programID, "viewMatrix");
	//shaderProgram.projectionMatrixID = glGetUniformLocation(shaderProgram.programID, "projectionMatrix");
	shaderProgram.mvpMatrixID = glGetUniformLocation(shaderProgram.programID, "vpMatrix");

	//glUniformMatrix4fv(shaderProgram.modelMatrixID, 1, GL_FALSE, &glm::mat4(1)[0][0]);
	//glUniformMatrix4fv(shaderProgram.viewMatrixID, 1, GL_FALSE, &glm::mat4(1)[0][0]);
	//glUniformMatrix4fv(shaderProgram.projectionMatrixID, 1, GL_FALSE, &glm::mat4(1)[0][0]);
	glUniformMatrix4fv(shaderProgram.mvpMatrixID, 1, GL_FALSE, &glm::mat4(1)[0][0]);

	shaderProgram.worldOffsetID = glGetUniformLocation(shaderProgram.programID, "worldOffset");
	
	shaderProgram.loaded = true;
}

void useShader(ShaderProgram & shader) {
	if (shader.loaded) {
		glUseProgram(shader.programID);
	}
}


ShaderProgram::ShaderProgram()
{
	debugPrint("|----ShaderProgram was created!");
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(programID);
	debugPrint("|----ShaderProgram was destroyed!");
}
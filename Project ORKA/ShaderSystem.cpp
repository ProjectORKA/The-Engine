
#include "ShaderSystem.hpp"

void ShaderSystem::create() {

	//must be initialized before any shaders are loaded
	uniforms.create();

	vertexShader.load(VertexShaderType,
	"#version 330 core\n\
	in vec3 vertex; \n\
	in vec2 uvs; \n\
	uniform mat4 vpMatrix; \n\
	uniform mat4 mMatrix; \n\
	out vec2 textureCoordinate; \n\
	void main()\n\
	{\n\
		gl_Position = vpMatrix * mMatrix * vec4(vertex, 1); \n\
		textureCoordinate = uvs; \n\
	}"
	);
	fragmentShader.load(FragmentShaderType,
	"#version 330 core\n\
	in vec2 textureCoordinate; \n\
	out vec4 color;;\n\
	uniform sampler2D texture0;\n\
	void main()\n\
	{\n\
	  color = vec4(texture(texture0, textureCoordinate).rgb,1.0f);\n\
	  //color = vec4(UV,0.0f,1.0f);\n\
	}"
	);
	add(vertexShader, fragmentShader, "default");
	vertexShader.unload();
	fragmentShader.unload();

	vertexShader.loadFromFile(VertexShaderType, "Data/shaders/primitive.vert");
	fragmentShader.loadFromFile(FragmentShaderType, "Data/shaders/primitive.frag");
	add(vertexShader, fragmentShader, "primitive");
	vertexShader.unload();
	fragmentShader.unload();

	vertexShader.loadFromFile(VertexShaderType, "Data/shaders/debug.vert");
	fragmentShader.loadFromFile(FragmentShaderType, "Data/shaders/debug.frag");
	add(vertexShader, fragmentShader, "debug");
	vertexShader.unload();
	fragmentShader.unload();

	vertexShader.loadFromFile(VertexShaderType, "Data/shaders/spaceShip.vert");
	fragmentShader.loadFromFile(FragmentShaderType, "Data/shaders/spaceShip.frag");
	add(vertexShader, fragmentShader, "spaceShip");
	vertexShader.unload();
	fragmentShader.unload();

	vertexShader.loadFromFile(VertexShaderType, "Data/shaders/sky.vert");
	fragmentShader.loadFromFile(FragmentShaderType, "Data/shaders/sky.frag");
	add(vertexShader, fragmentShader, "sky");
	vertexShader.unload();
	fragmentShader.unload();

	vertexShader.loadFromFile(VertexShaderType, "Data/shaders/postProcess.vert");
	fragmentShader.loadFromFile(FragmentShaderType, "Data/shaders/postProcess.frag");
	add(vertexShader, fragmentShader, "postProcess");
	vertexShader.unload();
	fragmentShader.unload();
}
void ShaderSystem::add(Shader& vertexShader, Shader& fragmentShader, Name name)
{
	shaderPrograms.emplace_back();
	shaderPrograms.back().load(vertexShader, fragmentShader, uniforms);
	shaderNames[name] = shaderPrograms.size() - 1;
}
void ShaderSystem::useShader(Index shaderID)
{
	shaderPrograms[shaderID].use(uniforms);
}
void ShaderSystem::useShader(Name name)
{
	auto it = shaderNames.find(name);
	if (it != shaderNames.end()) {
		shaderPrograms[it->second].use(uniforms);
	}
}
void ShaderSystem::destroy() {
	for (ShaderProgram& shaderProgram : shaderPrograms) {
		shaderProgram.unload();
	}
	shaderPrograms.clear();
	shaderNames.clear();
	uniforms.destroy();
}

void Shader::loadFromFile(Index shaderType, Path path) {
#ifdef GRAPHICS_API_OPENGL
	String shaderCode;
	
	loadString(shaderCode, path);

	load(shaderType, shaderCode);

#endif // GRAPHICS_API_OPENGL
}
void Shader::load(Index shaderType, String shaderCode)
{
#ifdef GRAPHICS_API_OPENGL
	shaderID = glCreateShader(shaderType);

	char const* SourcePointer = shaderCode.c_str();
	glShaderSource(shaderID, 1, &SourcePointer, NULL);
	glCompileShader(shaderID);

	GLint Result = GL_FALSE;
	GLint InfoLogLength = 0;

	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		Vector<char> ShaderErrorMessage(InfoLogLength++);
		glGetShaderInfoLog(shaderID, InfoLogLength, NULL, &ShaderErrorMessage[0]);
		printf("%s\n", &ShaderErrorMessage[0]);
	}
#endif // GRAPHICS_API_OPENGL
}
void Shader::unload() {
#ifdef GRAPHICS_API_OPENGL
	glDeleteShader(shaderID);
#endif // GRAPHICS_API_OPENGL
}

void ShaderProgram::use(Uniforms& uniforms) {
#ifdef GRAPHICS_API_OPENGL
	if (loaded) {
		glUseProgram(programID);

		for (auto & bVariable : uniforms.bools) {
			glUniform1i(uniformNameToID[bVariable.first.data], bVariable.second);
		}

		for (auto& iVariable : uniforms.ints) {
			glUniform1i(uniformNameToID[iVariable.first.data], iVariable.second);
		}

		for (auto& fVariable : uniforms.floats) {
			glUniform1f(uniformNameToID[fVariable.first.data], fVariable.second);
		}

		for (auto & vec3Variable : uniforms.vec3s) {
			glUniform3f(uniformNameToID[vec3Variable.first.data], vec3Variable.second.x, vec3Variable.second.y, vec3Variable.second.z);
		}

		for (auto & vec4Variable : uniforms.vec4s) {
			glUniform4f(uniformNameToID[vec4Variable.first.data], vec4Variable.second.x, vec4Variable.second.y, vec4Variable.second.z, vec4Variable.second.w);
		}

		for (auto & matVariable : uniforms.matrices) {
			glUniformMatrix4fv(uniformNameToID[matVariable.first.data], 1, GL_FALSE, glm::value_ptr(matVariable.second));
		}
	}
	else {
		logError("ShaderProgram not loaded!");
	}
#endif // GRAPHICS_API_OPENGL
};
void ShaderProgram::load(Shader& vertexShader, Shader& fragmentShader, Uniforms & uniforms) {
	if (loaded) {
		logError("ShaderProgram already loaded!");
	}
	else {
#ifdef GRAPHICS_API_OPENGL

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
		for (auto & b : uniforms.bools) {
			uniformNameToID[b.first.data] = glGetUniformLocation(programID, b.first.data);
		}

		for (auto & i : uniforms.ints) {
			uniformNameToID[i.first.data] = glGetUniformLocation(programID, i.first.data);
		}

		for (auto & f : uniforms.floats) {
			uniformNameToID[f.first.data] = glGetUniformLocation(programID, f.first.data);
		}

		for (auto & v3 : uniforms.vec3s) {
			uniformNameToID[v3.first.data] = glGetUniformLocation(programID, v3.first.data);
		}

		for (auto & v4 : uniforms.vec4s) {
			uniformNameToID[v4.first.data] = glGetUniformLocation(programID, v4.first.data);
		}

		for (auto & m : uniforms.matrices) {
			uniformNameToID[m.first.data] = glGetUniformLocation(programID, m.first.data);
		}

		glUniform1i(glGetUniformLocation(programID, "baseColor"), 0);
		//glUniform1i(glGetUniformLocation(programID, "normal"), 1);
		//glUniform1i(glGetUniformLocation(programID, "roughness"), 2);
		//glUniform1i(glGetUniformLocation(programID, "metalness"), 3);

		//for (auto& t : uniforms.textures) {
		//	uniformNameToID[t.first.data] = 
		//}

#endif // GRAPHICS_API_OPENGL

		loaded = true;
	}
}
void ShaderProgram::unload()
{
	if (loaded) {
#ifdef GRAPHICS_API_OPENGL
	glDeleteProgram(programID);
#endif // GRAPHICS_API_OPENGL
	}
	else {
		logDebug("ShaderProgram already unloaded!");
	}
}
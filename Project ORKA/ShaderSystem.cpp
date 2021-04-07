
#include "ShaderSystem.hpp"

void ShaderSystem::create() {
	uniforms.create();
	loadDefaultShader();
}
void ShaderSystem::destroy() {

	for (ShaderProgram& shaderProgram : shaderPrograms) {
		shaderProgram.unload();
	}
	shaderPrograms.clear();
	shaderNames.clear();
	uniforms.destroy();

}
void ShaderSystem::use(Name name)
{
	auto it = shaderNames.find(name);
	if (it != shaderNames.end()) {
		use(it->second);
	}
	else {
		//try loading the shader
		loadShaderProgram(name);
		it = shaderNames.find(name);
		if (it != shaderNames.end()) {
			use(it->second);
		}
		else {
			logError("Shader could not be found!");
		}
	}
}
void ShaderSystem::loadDefaultShader()
{
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
}
void ShaderSystem::use(Index shaderProgramID)
{
	currentShaderProgramID = shaderProgramID;
	currentShaderProgram().use();
	currentShaderProgram().updateUniforms();
}
void ShaderSystem::loadShaderProgram(Name name)
{
	logEvent(String("Loading ShaderProgram: (").append(name.data).append(")"));
	vertexShader.loadFromFile(VertexShaderType, String("Data/shaders/").append(name.data).append(".vert"));
	fragmentShader.loadFromFile(FragmentShaderType, String("Data/shaders/").append(name.data).append(".frag"));
	if (vertexShader.isValid & fragmentShader.isValid) {
		add(vertexShader, fragmentShader, name.data);
	}
	else {
		logError("ShaderProgram could not be created! Shaders didnt validate!");
	}
	vertexShader.unload();
	fragmentShader.unload();
}
void ShaderSystem::add(Shader& vertexShader, Shader& fragmentShader, Name name)
{
	shaderPrograms.emplace_back();
	shaderPrograms.back().create(vertexShader, fragmentShader, uniforms);
	shaderNames[name] = shaderPrograms.size() - 1;
}

ShaderProgram& ShaderSystem::currentShaderProgram()
{
	return shaderPrograms[currentShaderProgramID];
}

//uniforms
//void ShaderSystem::setBool(Name name, Bool value)
//{
//	glUniform1i(currentShaderProgram().uniformNameToID[name], value);
//	uniforms.setBool(name, value);
//}
//void ShaderSystem::setInt(Name name, Int value)
//{
//	glUniform1i(currentShaderProgram().uniformNameToID[name], value);
//	uniforms.setInt(name, value);
//}
//void ShaderSystem::setVec3(Name name, Vec3 value)
//{
//	glUniform3f(currentShaderProgram().uniformNameToID[name], value.x, value.y, value.z);
//	uniforms.setVec3(name, value);
//}
//void ShaderSystem::setVec4(Name name, Vec4 value)
//{
//	glUniform4f(currentShaderProgram().uniformNameToID[name], value.x, value.y, value.z, value.w);
//	uniforms.setVec4(name, value);
//}
//void ShaderSystem::setFloat(Name name, Float value)
//{
//	glUniform1i(currentShaderProgram().uniformNameToID[name], value);
//	uniforms.setFloat(name, value);
//}
//void ShaderSystem::setMatrix(Name name, Matrix value)
//{
//	glUniformMatrix4fv(currentShaderProgram().uniformNameToID[name], 1, GL_FALSE, glm::value_ptr(value));
//	uniforms.setMatrix(name, value);
//}
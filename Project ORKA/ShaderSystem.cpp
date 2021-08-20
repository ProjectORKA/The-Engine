
#include "ShaderSystem.hpp"

void ShaderSystem::rebuild()
{
	for (ShaderProgram& shaderProgram : shaderPrograms) {
		shaderProgram.rebuild();
	}
}
void ShaderSystem::create() {
	uniforms.create();
	loadDefaultShader();
}
void ShaderSystem::destroy() {

	for (ShaderProgram& shaderProgram : shaderPrograms) {
		shaderProgram.destroy();
	}
	shaderPrograms.clear();
	shaderNames.clear();
	uniforms.destroy();

}
void ShaderSystem::add(Name name)
{
	shaderPrograms.emplace_back();
	shaderPrograms.back().create(name, uniforms);
	currentShaderProgramID = shaderPrograms.size() - 1;
	shaderNames[name] = currentShaderProgramID;
}
void ShaderSystem::use(Name name)
{

	auto it = shaderNames.find(name);
	if (it != shaderNames.end()) {
		use(it->second);
	}
	else {
		//try loading the shader
		add(name);
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
	//create default vertex shader
	Shader vertexShader;
	vertexShader.loadShaderCode(ShaderType::vertex, String(
		"#version 450\n\
		in vec3 vertex; \n\
		in vec2 uvs; \n\
		layout(std140, binding = 0) uniform GlobalUniforms\n\
		{\n\
			mat4 mMatrix;\n\
			mat4 vpMatrix;\n\
		\n\
		vec4 worldOffset; \n\
		vec4 cameraVector;\n\
		vec4 chunkOffsetVector;\n\
		\n\
		float tint; \n\
		float fade;\n\
		float time;\n\
		float cameraHeight;\n\
		\n\
		bool distortion;\n\
		};\n\
		\n\
		uniform sampler2D texture0;\n\
		\n\
		out vec2 textureCoordinate; \n\
		void main()\n\
		{\n\
			gl_Position = vpMatrix * mMatrix * vec4(vertex, 1); \n\
			textureCoordinate = uvs; \n\
		}"
	));

	//create default fragment shader
	Shader fragmentShader;
	fragmentShader.loadShaderCode(ShaderType::fragment, String(
		"#version 450\n\
	in vec2 textureCoordinate; \n\
	out vec4 color;;\n\
	layout(std140, binding = 0) uniform GlobalUniforms\n\
	{\n\
		mat4 mMatrix;\n\
		mat4 vpMatrix;\n\
		\n\
		vec4 worldOffset; \n\
		vec4 cameraVector;\n\
		vec4 chunkOffsetVector;\n\
		\n\
		float tint; \n\
		float fade;\n\
		float time;\n\
		float cameraHeight;\n\
		\n\
		bool distortion;\n\
	};\n\
	\n\
	uniform sampler2D texture0;\n\
	void main()\n\
	{\n\
	  color = vec4(texture(texture0, textureCoordinate).rgb,1.0f);\n\
	  //color = vec4(UV,0.0f,1.0f);\n\
	}"
	));

	add(vertexShader, fragmentShader, "default");
	vertexShader.unload();
	fragmentShader.unload();
}
Index ShaderSystem::getShaderID(Name name) {
	use(name);
	return currentShaderProgramID;
}
void ShaderSystem::use(Index shaderProgramID)
{
	currentShaderProgramID = shaderProgramID;
	currentShaderProgram().select();
}
ShaderProgram& ShaderSystem::currentShaderProgram()
{
	return shaderPrograms[currentShaderProgramID];
}
void ShaderSystem::add(Shader& vertexShader, Shader& fragmentShader, Name name)
{
	shaderPrograms.emplace_back();
	shaderPrograms.back().create(vertexShader, fragmentShader, uniforms);
	currentShaderProgramID = shaderPrograms.size() - 1;
	shaderNames[name] = currentShaderProgramID;
}
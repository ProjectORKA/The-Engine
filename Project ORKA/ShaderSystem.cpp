
#include "ShaderSystem.hpp"

String uniformName = "/uniforms.glsl";

void ShaderSystem::create(Engine & engine) {
	uniforms.create(engine);
	loadDefaultShader();
}
void ShaderSystem::rebuild()
{
	for (auto& name : shaderNames) {
		shaderPrograms[name.second].destroy();
	}
	shaderNames.clear();
}
void ShaderSystem::destroy() {
	for (ShaderProgram& shaderProgram : shaderPrograms) {
		shaderProgram.destroy();
	}
	shaderPrograms.clear();
	shaderNames.clear();
	uniforms.destroy();
}
void ShaderSystem::loadDefaultShader()
{
	//create default vertex shader
	String defaultvertexShaderCode = uniforms.uniformBlockShaderCode;
	defaultvertexShaderCode.append(
"\n\
void main()\n\
{\n\
	gl_Position = pMatrix * vMatrix * mMatrix * vec4(vertex, 1); \n\
}"
	);

	Shader vertexShader;
	vertexShader.loadShaderCode(ShaderType::vertex, defaultvertexShaderCode);

	//create default fragment shader
	String defaultFragmentShaderCode = uniforms.uniformBlockShaderCode;
	defaultFragmentShaderCode.append(
		"\n\
\n\
out vec4 color;\n\
\n\
void main()\n\
{\n\
	//color = vec4(1.0f,0.0f,1.0f,1.0f);\n\
}"
	);
	Shader fragmentShader;
	fragmentShader.loadShaderCode(ShaderType::fragment, defaultFragmentShaderCode);

	add(vertexShader, fragmentShader, "default");
	vertexShader.destroy();
	fragmentShader.destroy();
}
Index ShaderSystem::use(Index shaderProgramID)
{
	currentShaderProgramID = shaderProgramID;
	currentShaderProgram().select();
	return currentShaderProgramID;
}
void ShaderSystem::add(Engine engine, Name name)
{
	shaderPrograms.emplace_back();
	shaderPrograms.back().create(engine, name, uniforms);
	currentShaderProgramID = shaderPrograms.size() - 1;
	shaderNames[name] = currentShaderProgramID;
}
Index ShaderSystem::use(Engine engine, Name name)
{
	auto it = shaderNames.find(name);
	if (it != shaderNames.end()) {
		use(it->second);
	}
	else {
		//try loading the shader
		add(engine, name);
		it = shaderNames.find(name);
		if (it != shaderNames.end()) {
			use(it->second);
		}
		else {
			logError("Shader could not be found!");
		}
	}
	return currentShaderProgramID;
}
ShaderProgram& ShaderSystem::currentShaderProgram()
{
	return shaderPrograms[currentShaderProgramID];
}
Index ShaderSystem::getShaderID(Engine& engine, Name name) {
	use(engine,name);
	return currentShaderProgramID;
}
void ShaderSystem::add(Shader& vertexShader, Shader& fragmentShader, Name name)
{
	shaderPrograms.emplace_back();
	shaderPrograms.back().create(vertexShader, fragmentShader, uniforms);
	currentShaderProgramID = shaderPrograms.size() - 1;
	shaderNames[name] = currentShaderProgramID;
}
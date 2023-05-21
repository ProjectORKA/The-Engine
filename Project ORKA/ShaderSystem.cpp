
#include "ShaderSystem.hpp"
#include "GraphicsAPI.hpp"

String uniformName = "/uniforms.glsl";

void ShaderSystem::create(ResourceManager& resourceManager)
{
	uniforms.create(resourceManager);
	loadDefaultShader();
}

void ShaderSystem::rebuild()
{
	for(const auto& name : shaderNames)
	{
		shaderPrograms[name.second].destroy();
	}
	shaderNames.clear();
}

void ShaderSystem::destroy()
{
	for(ShaderProgram& shaderProgram : shaderPrograms)
	{
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
	defaultvertexShaderCode.append("\n\
void main()\n\
{\n\
	gl_Position = pMatrix * vMatrix * mMatrix * vec4(vertex, 1); \n\
}");

	Shader vertexShader;
	vertexShader.loadShaderCode(ShaderType::Vertex, defaultvertexShaderCode);

	//create default fragment shader
	String defaultFragmentShaderCode = uniforms.uniformBlockShaderCode;
	defaultFragmentShaderCode.append("\n\
\n\
out vec4 color;\n\
\n\
void main()\n\
{\n\
	//color = vec4(1.0f,0.0f,1.0f,1.0f);\n\
}");
	Shader fragmentShader;
	fragmentShader.loadShaderCode(ShaderType::Fragment, defaultFragmentShaderCode);

	add(vertexShader, fragmentShader, "default");
	vertexShader.destroy();
	fragmentShader.destroy();
}

Index ShaderSystem::use(const Index shaderProgramId)
{
	currentShaderProgramId = shaderProgramId;
	currentShaderProgram().select();
	return currentShaderProgramId;
}

void ShaderSystem::add(ResourceManager& resourceManager, const Name& name)
{
	shaderPrograms.emplace_back();
	shaderPrograms.back().create(resourceManager, name, uniforms);
	currentShaderProgramId = toUIntSafe(shaderPrograms.size() - 1);
	shaderNames[name] = currentShaderProgramId;
}

Index ShaderSystem::use(ResourceManager& resourceManager, const Name& name)
{
	auto it = shaderNames.find(name);
	if(it != shaderNames.end())
	{
		use(it->second);
	}
	else
	{
		//try loading the shader
		add(resourceManager, name);
		it = shaderNames.find(name);
		if(it != shaderNames.end())
		{
			use(it->second);
		}
		else
		{
			logError("Shader could not be found!");
		}
	}
	return currentShaderProgramId;
}

ShaderProgram& ShaderSystem::currentShaderProgram()
{
	return shaderPrograms[currentShaderProgramId];
}

Index ShaderSystem::getShaderId(ResourceManager& resourceManager, const Name& name)
{
	use(resourceManager, name);
	return currentShaderProgramId;
}

void ShaderSystem::add(const Shader& vertexShader, const Shader& fragmentShader, const Name& name)
{
	shaderPrograms.emplace_back();
	shaderPrograms.back().create(vertexShader, fragmentShader, uniforms);
	currentShaderProgramId = shaderPrograms.size() - 1;
	shaderNames[name] = currentShaderProgramId;
}
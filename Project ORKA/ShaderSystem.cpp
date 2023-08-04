#include "ShaderSystem.hpp"
#include <ranges>
#include "Debug.hpp"
#include "ResourceManager.hpp"

String uniformName = "/uniforms.glsl";

void ShaderSystem::rebuild()
{
	for(const auto& val : shaderNames | std::views::values) shaderPrograms[val].destroy();
	shaderNames.clear();
}

void ShaderSystem::destroy()
{
	for(ShaderProgram& shaderProgram : shaderPrograms) shaderProgram.destroy();
	shaderPrograms.clear();
	shaderNames.clear();
	uniforms.destroy();
}

void ShaderSystem::loadDefaultShader()
{
	String vertexShaderCode = uniforms.getUniformShaderCode();
	vertexShaderCode.append("\n\nvoid main(){\ngl_Position = pMatrix * vMatrix * mMatrix * vec4(vertex, 1);\n}");

	String fragmentShaderCode = uniforms.getUniformShaderCode();
	fragmentShaderCode.append("\n\nout vec4 color;\n\nvoid main(){\ncolor = vec4(1.0f,0.0f,1.0f,1.0f);\n}");

	add("default", vertexShaderCode, fragmentShaderCode);
}

Index ShaderSystem::use(const Index shaderId)
{
	currentShaderProgramId = shaderId;
	currentShaderProgram().use();
	return currentShaderProgramId;
}

ShaderProgram& ShaderSystem::currentShaderProgram()
{
	return shaderPrograms[currentShaderProgramId];
}

void ShaderSystem::create(const ResourceManager& resourceManager)
{
	uniforms.create(resourceManager);
	loadDefaultShader();
}

void ShaderSystem::add(ResourceManager& resourceManager, const Name& name)
{
	const Path vertexShaderPath   = resourceManager.getVertexShaderResourcePath(name);
	const Path fragmentShaderPath = resourceManager.getFragmentShaderResourcePath(name);

	if(!doesPathExist(vertexShaderPath)) logError("Vertex shader not found");
	if(!doesPathExist(fragmentShaderPath)) logError("Fragment shader not found");

	String vertexShaderCode   = uniforms.getUniformShaderCode();
	String fragmentShaderCode = uniforms.getUniformShaderCode();

	vertexShaderCode.append(loadString(vertexShaderPath));
	fragmentShaderCode.append(loadString(fragmentShaderPath));

	add(name, vertexShaderCode, fragmentShaderCode);
}

Index ShaderSystem::use(ResourceManager& resourceManager, const Name& name)
{
	auto it = shaderNames.find(name);
	if(it != shaderNames.end()) use(it->second);
	else
	{
		// try loading the shader
		add(resourceManager, name);
		it = shaderNames.find(name);
		if(it != shaderNames.end()) use(it->second);
		else logError("Shader could not be found!");
	}
	return currentShaderProgramId;
}

Index ShaderSystem::getShaderId(ResourceManager& resourceManager, const Name& name)
{
	use(resourceManager, name);
	return currentShaderProgramId;
}

void ShaderSystem::add(const Name& name, const String& vertexShaderCode, const String& fragmentShaderCode)
{
	// reserve data for shader
	shaderPrograms.emplace_back();
	// create shader
	shaderPrograms.back().create(name, vertexShaderCode, fragmentShaderCode);
	// register the new shader
	currentShaderProgramId = static_cast<UInt>(shaderPrograms.size()) - 1;
	shaderNames[name]      = currentShaderProgramId;
}

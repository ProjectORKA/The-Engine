#include "ShaderSystem.hpp"
#include "Debug.hpp"
#include "ResourceManager.hpp"
#include "Renderer.hpp"
#include "FileSystem.hpp"

String uniformName = "/uniforms.glsl";

namespace
{
	constexpr const char* IncludePrefix = "//! #include \"";

	Bool isEngineUniformInclude(const String& includeName)
	{
		return includeName == "uniforms.glsl" || includeName == "uniformsCompute.glsl";
	}

	String expandShaderIncludes(const String& source, const Path& shaderDirectory)
	{
		String expanded;
		std::istringstream stream(source);
		String line;
		while (std::getline(stream, line))
		{
			if (line.rfind(IncludePrefix, 0) == 0)
			{
				const auto endQuote = line.find('"', std::strlen(IncludePrefix));
				if (endQuote != String::npos)
				{
					const String includeName = line.substr(std::strlen(IncludePrefix), endQuote - std::strlen(IncludePrefix));
					if (!isEngineUniformInclude(includeName))
					{
						const Path includePath = shaderDirectory / includeName;
						expanded.append(expandShaderIncludes(loadString(includePath), shaderDirectory));
						continue;
					}
				}
			}

			expanded.append(line);
			expanded.push_back('\n');
		}
		return expanded;
	}
}

void ShaderSystem::rebuild(Renderer& renderer)
{
	for(const auto& val : shaderNames | std::views::values) shaderPrograms[val].destroy(renderer.openGlContext);
	shaderNames.clear();
}

void ShaderSystem::destroy(Renderer& renderer)
{
	for(ShaderProgram& shaderProgram : shaderPrograms) shaderProgram.destroy(renderer.openGlContext);
	shaderPrograms.clear();
	shaderNames.clear();
	uniforms.destroy(renderer.openGlContext);
}

void ShaderSystem::loadDefaultShader(Renderer& renderer)
{
	String vertexShaderCode = uniforms.getUniformShaderCode();
	vertexShaderCode.append("\n\nvoid main(){\ngl_Position = pMatrix * vMatrix * mMatrix * vec4(vertex, 1);\n}");

	String fragmentShaderCode = uniforms.getUniformShaderCode();
	fragmentShaderCode.append("\n\nout vec4 color;\n\nvoid main(){\ncolor = vec4(1.0f,0.0f,1.0f,1.0f);\n}");

	add(renderer, "default", vertexShaderCode, fragmentShaderCode);
}

Index ShaderSystem::use(Renderer& renderer, const Index shaderId)
{
	currentShaderProgramId = shaderId;
	currentShaderProgram().use(renderer.openGlContext);
	return currentShaderProgramId;
}

ShaderProgram& ShaderSystem::currentShaderProgram()
{
	return shaderPrograms[currentShaderProgramId];
}

void ShaderSystem::create(Renderer& renderer)
{
	uniforms.create(renderer.openGlContext);
	loadDefaultShader(renderer);
}

void ShaderSystem::add(Renderer& renderer, const Name& name)
{
	if (resourceManager.hasComputeShaderResource(name) && !resourceManager.hasVertexShaderResource(name))
	{
		const Path computeShaderPath = resourceManager.getComputeShaderResourcePath(name);
		if (!doesPathExist(computeShaderPath)) logError("Compute shader not found");

		String computeShaderCode = uniforms.getComputeUniformShaderCode();
		computeShaderCode.append(expandShaderIncludes(loadString(computeShaderPath), computeShaderPath.parent_path()));
		addCompute(renderer, name, computeShaderCode);
		return;
	}

	const Path vertexShaderPath   = resourceManager.getVertexShaderResourcePath(name);
	const Path fragmentShaderPath = resourceManager.getFragmentShaderResourcePath(name);

	if(!doesPathExist(vertexShaderPath)) logError("Vertex shader not found");
	if(!doesPathExist(fragmentShaderPath)) logError("Fragment shader not found");

	String vertexShaderCode   = uniforms.getUniformShaderCode();
	String fragmentShaderCode = uniforms.getUniformShaderCode();

	vertexShaderCode.append(loadString(vertexShaderPath));
	fragmentShaderCode.append(loadString(fragmentShaderPath));

	add(renderer, name, vertexShaderCode, fragmentShaderCode);
}

Index ShaderSystem::use(Renderer& renderer, const Name& name)
{
	auto it = shaderNames.find(name);
	if(it != shaderNames.end()) use(renderer, it->second);
	else
	{
		// try loading the shader
		add(renderer, name);
		it = shaderNames.find(name);
		if(it != shaderNames.end()) use(renderer, it->second);
		else logError("Shader could not be found!");
	}
	return currentShaderProgramId;
}

Index ShaderSystem::getShaderId(Renderer& renderer, const Name& name)
{
	use(renderer, name);
	return currentShaderProgramId;
}

void ShaderSystem::add(Renderer& renderer, const Name& name, const String& vertexShaderCode, const String& fragmentShaderCode)
{
	// reserve data for shader
	shaderPrograms.emplace_back();
	// create shader
	shaderPrograms.back().create(renderer.openGlContext, name, vertexShaderCode, fragmentShaderCode);
	// register the new shader
	currentShaderProgramId = static_cast<UInt>(shaderPrograms.size()) - 1;
	shaderNames[name]      = currentShaderProgramId;
}

void ShaderSystem::addCompute(Renderer& renderer, const Name& name, const String& computeShaderCode)
{
	shaderPrograms.emplace_back();
	shaderPrograms.back().createCompute(renderer.openGlContext, name, computeShaderCode);
	currentShaderProgramId = static_cast<UInt>(shaderPrograms.size()) - 1;
	shaderNames[name]      = currentShaderProgramId;
}

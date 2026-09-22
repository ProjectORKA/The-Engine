#include "AiPaths.hpp"

#include "../File.hpp"
#include "../ResourceManager.hpp"
#include "AiConfig.hpp"

#include <filesystem>

Path AiPaths::aiRoot()
{
	return resourceManager.getOrkaCachePath() / "ai";
}

String AiPaths::activeRuntimeTag()
{
	const Path tagFile = aiRoot() / "active-runtime-tag.txt";
	if (!std::filesystem::exists(tagFile)) return AiConfig::llamaCppReleaseTag;

	InFile file(tagFile);
	if (!file.isOpen) return AiConfig::llamaCppReleaseTag;

	String line;
	if (file.readLine(line) && !line.empty()) return line;
	return AiConfig::llamaCppReleaseTag;
}

void AiPaths::setActiveRuntimeTag(const String& tag)
{
	std::filesystem::create_directories(aiRoot());
	OutFile file(aiRoot() / "active-runtime-tag.txt");
	if (!file.isOpen) return;
	file.write(tag.c_str(), tag.size());
}

Path AiPaths::runtimeRoot()
{
	return aiRoot() / "runtime" / activeRuntimeTag();
}

Path AiPaths::modelsRoot()
{
	return aiRoot() / "models";
}

Path AiPaths::consentFile()
{
	return aiRoot() / "consent.json";
}

Path AiPaths::runtimeZip(const String& zipFileName)
{
	return aiRoot() / "downloads" / zipFileName;
}

Path AiPaths::defaultModelFile()
{
	return modelsRoot() / (AiConfig::defaultModelId + String(".gguf"));
}

Path AiPaths::stateFile()
{
	return aiRoot() / "runtime-state.json";
}

void AiPaths::ensureLayout()
{
	std::filesystem::create_directories(aiRoot());
	std::filesystem::create_directories(runtimeRoot());
	std::filesystem::create_directories(modelsRoot());
	std::filesystem::create_directories(aiRoot() / "downloads");
}

#pragma once

#include "Basics.hpp"

using ResourceID = Index;

enum class ResourceType {
	Mesh,
	Texture,
	Shader
};

struct Resource {
	Name name;
	Path path;
	ResourceType type;
};

struct ResourceManager {
	Map<Name, Path> meshResources;
	Map<Name, Path> textureResources;
	Map<Name, Path> vertexShaderResources;
	Map<Name, Path> fragmentShaderResources;

	ResourceManager();
	void reloadAllResources();
	void loadResourcesFromFBXFiles();
};

extern ResourceManager resourceManager;

void destroyResourceManager();
void initializeResourcemanager();
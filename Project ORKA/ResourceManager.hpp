
#pragma once

#include "Basics.hpp"
#include "FileSystem.hpp"

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
	Path orkaDataLocation;
	Path orkaBinariesLocation;

	Map<Name, Path> meshResources;
	Map<Name, Path> textureResources;
	Map<Name, Path> vertexShaderResources;
	Map<Name, Path> fragmentShaderResources;

	ResourceManager(FileSystem& fileSystem);
	void reloadAllResources(FileSystem& fileSystem);
	void loadResourcesFromFBXFiles(FileSystem& fileSystem);
};
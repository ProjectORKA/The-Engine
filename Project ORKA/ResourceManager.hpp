
#pragma once

#include "Basics.hpp"
#include "FileSystem.hpp"

// TODO
// add file watcher that reloads resources if they were changed

struct ResourceManager {
	//paths
	Path orkaLocation;
	Path orkaDataLocation;
	Path orkaCacheLocation;
	Path orkaCachedMeshLocation;
	Path resourceManagerConfigPath;

	//resources
	Map<Name, Path> meshResources;
	Map<Name, Path> textureResources;
	Map<Name, Path> vertexShaderResources;
	Map<Name, Path> fragmentShaderResources;

	ResourceManager(FileSystem& fileSystem);
	void reloadAllResources(FileSystem& fileSystem);
	void loadResourcesFromFBXFiles(FileSystem& fileSystem);
};
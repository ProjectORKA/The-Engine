
#pragma once

#include "Basics.hpp"
#include "FileSystem.hpp"

// TODO
// add file watcher that reloads resources if they were changed

struct ResourceManager {
	Bool initialized = false;
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

	void init();
	void reloadAllResources();
	void addResource(Path path);
	void loadResourcesFromFBXFiles();
};


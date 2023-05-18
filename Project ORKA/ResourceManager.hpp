#pragma once

#include "Basics.hpp"
#include "FileSystem.hpp"

// TODO
// add file watcher that reloads resources if they were changed

struct ResourceManager {
	Bool initialized = false;

	void create();
	void reloadAllResources();
	void addResource(const Path& path);
	void loadResourcesFromFbxFiles();

	Path getOrkaDataLocation() const;
	Path getOrkaCacheLocation() const;
	Path getOrkaCachedMeshLocation() const;
	Path getResourceManagerConfigPath() const;

	Bool hasMeshResource(const Name& name) const;
	Bool hasTextureResource(const Name& name) const;
	Bool hasVertexShaderResource(const Name& name) const;
	Bool hasFragmentShaderResource(const Name& name) const;

	Path getMeshResourcePath(const Name& name);;
	Path getTextureResourcePath(const Name& name);;
	Path getVertexShaderResourcePath(const Name& name);;
	Path getFragmentShaderResourcePath(const Name& name);;

private:
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
};

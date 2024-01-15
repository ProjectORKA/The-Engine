#pragma once

#include "Basics.hpp"
#include "FileSystem.hpp"

// TODO
// add file watcher that reloads resources if they were changed

struct ResourceManagerConfig
{
	FileTime lastTimeFbxWasEdited;
	UInt     lastNumberOfFbxFiles;
};

struct ResourceManager
{
	void create();
	void reloadAllResources();
	void addResource(const Path& path);
	void loadResourcesFromFbxFiles() const;

	[[nodiscard]] Path getOrkaDataLocation() const;
	[[nodiscard]] Path getOrkaCacheLocation() const;
	[[nodiscard]] Path getOrkaCachedMeshLocation() const;
	[[nodiscard]] Path getResourceManagerConfigPath() const;

	[[nodiscard]] Bool hasMeshResource(const Name& name) const;
	[[nodiscard]] Bool hasTextureResource(const Name& name) const;
	[[nodiscard]] Bool hasVertexShaderResource(const Name& name) const;
	[[nodiscard]] Bool hasFragmentShaderResource(const Name& name) const;

	Path getMeshResourcePath(const Name& name);
	Path getTextureResourcePath(const Name& name);
	Path getVertexShaderResourcePath(const Name& name);
	Path getFragmentShaderResourcePath(const Name& name);

private:
	Bool initialized = false;
	// paths
	Path orkaLocation;
	Path orkaDataLocation;
	Path orkaCacheLocation;
	Path orkaCachedMeshLocation;
	Path resourceManagerConfigPath;

	// resources
	Map<Name, Path> meshResources;
	Map<Name, Path> textureResources;
	Map<Name, Path> vertexShaderResources;
	Map<Name, Path> fragmentShaderResources;
};

extern ResourceManager resourceManager;

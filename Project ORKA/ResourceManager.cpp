
#include "File.hpp"
#include "Scene.hpp"
#include "FileSystem.hpp"
#include "ResourceManager.hpp"
#include "FileTypes.hpp"

void ResourceManager::create()
{
	// make sure the resource manager is only initialized once
	if(initialized)
	{
		logError("Resource manager already initialized!");
	}
	else
	{
		// the "ORKA" path is the folder that contains the executable
		if(executablePath == "") logError("Executable path not set!");
		orkaPath = makeAbsolute(executablePath);

		// Check if the path ends with "Debug" or "Release"
		if (String folderName = getFolderName(orkaPath); folderName == "Debug" || folderName == "Release") {
			// ORKA is run in dev environment
			// change paths accordingly
			orkaPath = getParentFolder(orkaPath);
			orkaPath = getParentFolder(orkaPath);
			orkaPath = getParentFolder(orkaPath);
		    orkaPath /= "Project ORKA\\";
	    }
		
		// we will always have a "Data" and "Cache" folder
		// "Data" contains all resources like meshes, shaders, levels, textures, scrips, etc
		orkaDataPath = orkaPath;
		orkaDataPath.append("Data");
		// "Cache" contains data that ORKA will create itself, so things like uncompressed or encrypted files, configs, logs, temporary data, etc
		orkaCachePath = orkaPath;
		orkaCachePath.append("Cache");
		// meshes are uncompressed 3d objects that can be loaded straight into memory
		orkaCachedMeshPath = orkaCachePath;
		orkaCachedMeshPath.append("Meshes");
		// the resource manager config keeps track of file updates
		resourceManagerConfigPath = makeAbsolute(Path(orkaCachePath).append("config").append("resourceManager.bin"));

		// setup data folder
		if(!doesPathExist(orkaDataPath)) createDirectory(orkaDataPath);
		
		// setup cache folder
		if(!doesPathExist(orkaCachePath)) createDirectory(orkaCachePath);

		initialized = true;

		reloadAllResources();
	}
}

void ResourceManager::reloadAllResources()
{
	loadResourcesFromFbxFiles();

	Vector<Path> paths;

	// get all data files
	paths.clear();
	const Vector<String> fileTypes = {".fbx", ".mesh", ".png", ".bmp", ".exr", ".hdr", ".jpg", ".vert", ".frag"};
	paths                          = getAllFilesInDirectory(orkaDataPath, fileTypes);
	for(auto p : paths) addResource(p);

	// get all cached files
	paths.clear();
	paths = getAllFilesInDirectory(orkaCachePath, fileTypes);
	for(auto p : paths) addResource(p);
}

void ResourceManager::loadResourcesFromFbxFiles() const
{
	// get all paths to fbx files
	Vector<Path> paths;
	paths = getAllFilesInDirectory(orkaDataPath, Vector<String>{".fbx"});

	ResourceManagerConfig newConfig;
	newConfig.lastTimeFbxWasEdited = getLastWrittenTimeOfFiles(paths);
	newConfig.lastNumberOfFbxFiles = paths.size();

	// check if file exists
	if(doesFileExist(resourceManagerConfigPath))
	{
		ResourceManagerConfig oldConfig;

		// read contents and check if newer
		InFile resourceManagerConfigFile(resourceManagerConfigPath);
		if(resourceManagerConfigFile.isOpen)
		{
			resourceManagerConfigFile.read(reinterpret_cast<Char*>(&oldConfig), sizeof(ResourceManagerConfig));
		}
		else
		{
			logError("Resource manager config file could not be loaded!");
		}

		if(newConfig.lastNumberOfFbxFiles == oldConfig.lastNumberOfFbxFiles && newConfig.lastTimeFbxWasEdited <= oldConfig.lastTimeFbxWasEdited) return;
	}

	for(auto& path : paths)
	{
		Scene scene;
		scene.loadFBX(path);
	}

	OutFile resourceManagerConfigFile(resourceManagerConfigPath);
	if(resourceManagerConfigFile.isOpen)
	{
		resourceManagerConfigFile.write(reinterpret_cast<Char*>(&newConfig), sizeof(ResourceManagerConfig));
	}
	else
	{
		logError("Resource manager config file could not be created!");
	}
}

Path ResourceManager::getOrkaDataPath() const
{
	if(!initialized) logError("Resource manager not initialized!");
	return orkaDataPath;
}

Path ResourceManager::getOrkaCachePath() const
{
	if(!initialized) logError("Resource manager not initialized!");
	return orkaCachePath;
}

void ResourceManager::addResource(const Path& path)
{
	const String extension = getFileExtension(path);
	if(extension == ".mesh")
	{
		meshResources[Name(path.stem().string())] = path;
		return;
	}
	if(extension == ".png" || extension == ".bmp" || extension == ".exr" || extension == ".hdr" || extension == ".jpg")
	{
		textureResources[Name(path.stem().string())] = path;
		return;
	}
	if(extension == ".vert")
	{
		vertexShaderResources[Name(path.stem().string())] = path;
		return;
	}
	if(extension == ".frag") fragmentShaderResources[Name(path.stem().string())] = path;
}

Path ResourceManager::getOrkaCachedMeshPath() const
{
	if(!initialized) logError("Resource manager not initialized!");
	return orkaCachedMeshPath;
}

Path ResourceManager::getResourceManagerConfigPath() const
{
	if(!initialized) logError("Resource manager not initialized!");
	return resourceManagerConfigPath;
}

Path ResourceManager::getMeshResourcePath(const Name& name)
{
	if(!initialized) logError("Resource manager not initialized!");
	if(!meshResources.contains(name)) logError("Mesh resource not found! (" + toString(name) + ")");
	return meshResources[name];
}

Bool ResourceManager::hasMeshResource(const Name& name) const
{
	if(!initialized) logError("Resource manager not initialized!");
	return meshResources.contains(name);
}

Path ResourceManager::getTextureResourcePath(const Name& name)
{
	if(!initialized) logError("Resource manager not initialized!");
	if(!textureResources.contains(name)) logError("Texture resource not found! (" + toString(name) + ")");
	return textureResources[name];
}

Bool ResourceManager::hasTextureResource(const Name& name) const
{
	if(!initialized) logError("Resource manager not initialized!");
	return textureResources.contains(name);
}

Path ResourceManager::getVertexShaderResourcePath(const Name& name)
{
	if(!initialized) logError("Resource manager not initialized!");
	if(!vertexShaderResources.contains(name)) logError("Vertex shader resource not found! (" + toString(name) + ")");
	return vertexShaderResources[name];
}

Path ResourceManager::getFragmentShaderResourcePath(const Name& name)
{
	if(!initialized) logError("Resource manager not initialized!");
	if(!fragmentShaderResources.contains(name)) logError("Fragment shader resource not found! (" + toString(name) + ")");
	return fragmentShaderResources[name];
}

Bool ResourceManager::hasVertexShaderResource(const Name& name) const
{
	if(!initialized) logError("Resource manager not initialized!");
	return vertexShaderResources.contains(name);
}

Bool ResourceManager::hasFragmentShaderResource(const Name& name) const
{
	if(!initialized) logError("Resource manager not initialized!");
	return fragmentShaderResources.contains(name);
}

ResourceManager resourceManager;

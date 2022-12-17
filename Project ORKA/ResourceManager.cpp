
#include "ResourceManager.hpp"
#include "Scene.hpp"
#include "FileSystem.hpp"


ResourceManager resourceManager;

void ResourceManager::init() {
	reloadAllResources();
}

void ResourceManager::loadResourcesFromFBXFiles() {
	//set path to executable path
	std::filesystem::current_path(fileSystem.executablePath.parent_path());
	
	//get all paths to fbx files
	Vector<Path> paths;
	paths = fileSystem.getAllFilesInDirectory(std::filesystem::current_path(), { ".fbx" });

	//get the last edited times and the stored value
	FileTime lastTimeFbxWasEdited = fileSystem.getLastWrittenTimeOfFiles(paths);
	Path resourceManagerConfigPath = "data/config/resourcemanager.bin";

	FileTime recordedLastTimeFbxWasEdited;
	//create a config file if necessary
	if (!fileSystem.doesPathExist(resourceManagerConfigPath)) {

		resourceManagerConfigPath = std::filesystem::absolute(resourceManagerConfigPath);

		OutFile resourceManagerConfigFile(resourceManagerConfigPath);
		if (resourceManagerConfigFile.isOpen) {
			resourceManagerConfigFile.write((char*) &recordedLastTimeFbxWasEdited, sizeof(recordedLastTimeFbxWasEdited));
		}
		else {
			logError("Resource manager config file could not be created!");
		}
	}
	//otherwise load from it
	{
		InFile resourceManagerConfigFile(resourceManagerConfigPath);
		if (resourceManagerConfigFile.isOpen) {
			resourceManagerConfigFile.read((char *) &recordedLastTimeFbxWasEdited, sizeof(recordedLastTimeFbxWasEdited));
		}
		else {
			logError("Resource manager config file could not be loaded!");
		}
	}

	if (lastTimeFbxWasEdited > recordedLastTimeFbxWasEdited) {
		for (auto filePath : paths) {
			if (fileSystem.lastWrittenTime(filePath) > recordedLastTimeFbxWasEdited) {
				Scene scene;
				scene.loadFBX(filePath);
			}
		}
	}
	else {
		logDebug("Fbx files did not update since last time executable was run!");
	}

	//save new last recorded file acces
	{
		OutFile resourceManagerConfigFile(resourceManagerConfigPath);
		if (resourceManagerConfigFile.isOpen) {
			resourceManagerConfigFile.write((char*)&lastTimeFbxWasEdited, sizeof(lastTimeFbxWasEdited));
		}
		else {
			logError("Resource manager config file could not be created!");
		}
	}
}

void ResourceManager::reloadAllResources() {
	
	loadResourcesFromFBXFiles();

	Vector<Path> paths;

	//load mesh files
	paths.clear();
	paths = fileSystem.getAllFilesInDirectory(std::filesystem::current_path(), { ".mesh" });
	for (auto p : paths) {
		meshResources[Name(p.stem().string())] = p;
	}

	//load textures
	paths.clear();
	paths = fileSystem.getAllFilesInDirectory(std::filesystem::current_path(), { ".png", ".bmp", ".exr", ".hdr", ".jpg" });
	for (auto p : paths) {
		textureResources[Name(p.stem().string())] = p;
	}

	//load vertex shaders
	paths.clear();
	paths = fileSystem.getAllFilesInDirectory(std::filesystem::current_path(), { ".vert" });
	for (auto p : paths) {
		vertexShaderResources[Name(p.stem().string())] = p;
	}

	//load fragment shaders
	paths.clear();
	paths = fileSystem.getAllFilesInDirectory(std::filesystem::current_path(), { ".frag" });
	for (auto p : paths) {
		fragmentShaderResources[Name(p.stem().string())] = p;
	}

}
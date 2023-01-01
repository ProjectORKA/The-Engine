
#include "ResourceManager.hpp"
#include "Scene.hpp"



ResourceManager::ResourceManager(FileSystem& fileSystem) {
	//get default ORKA folder which is always in the root drive of the executable
	Path orkaLocation = makeAbsolute(fileSystem.executablePath.root_directory()).append("ORKA");
	
	//setup data folder
	orkaDataLocation = orkaLocation;
	orkaDataLocation.append("Data");
	if (!doesPathExist(orkaDataLocation))createDirectory(orkaDataLocation);

	//setup binary folder
	orkaBinariesLocation = orkaLocation;
	orkaBinariesLocation.append("Bin");
	if (!doesPathExist(orkaBinariesLocation))createDirectory(orkaBinariesLocation);

	reloadAllResources(fileSystem);
}
void ResourceManager::reloadAllResources(FileSystem & fileSystem) {

	loadResourcesFromFBXFiles(fileSystem);

	Vector<Path> paths;

	//load mesh files
	paths.clear();
	paths = getAllFilesInDirectory(orkaBinariesLocation, { ".mesh" });
	for (auto p : paths) {
		meshResources[Name(p.stem().string())] = p;
	}

	//load textures
	paths.clear();
	paths = getAllFilesInDirectory(orkaDataLocation, { ".png", ".bmp", ".exr", ".hdr", ".jpg" });
	for (auto p : paths) {
		textureResources[Name(p.stem().string())] = p;
	}

	//load vertex shaders
	paths.clear();
	paths = getAllFilesInDirectory(orkaDataLocation, { ".vert" });
	for (auto p : paths) {
		vertexShaderResources[Name(p.stem().string())] = p;
	}

	//load fragment shaders
	paths.clear();
	paths = getAllFilesInDirectory(orkaDataLocation, { ".frag" });
	for (auto p : paths) {
		fragmentShaderResources[Name(p.stem().string())] = p;
	}

}
void ResourceManager::loadResourcesFromFBXFiles(FileSystem& fileSystem) {

	//get all paths to fbx files
	Vector<Path> paths;
	paths = getAllFilesInDirectory(orkaDataLocation, { ".fbx" });

	//get the last edited times and the stored value
	FileTime lastTimeFbxWasEdited = getLastWrittenTimeOfFiles(paths);
	Path resourceManagerConfigPath = makeAbsolute(Path(orkaBinariesLocation).append("config").append("resourceManager.bin"));

	FileTime recordedLastTimeFbxWasEdited;
	//create a config file if necessary
	if (!doesPathExist(resourceManagerConfigPath)) {

		OutFile resourceManagerConfigFile(resourceManagerConfigPath);
		if (resourceManagerConfigFile.isOpen) {
			resourceManagerConfigFile.write((char*)&recordedLastTimeFbxWasEdited, sizeof(recordedLastTimeFbxWasEdited));
		}
		else {
			logError("Resource manager config file could not be created!");
		}
	}
	//otherwise load from it
	{
		InFile resourceManagerConfigFile(resourceManagerConfigPath);
		if (resourceManagerConfigFile.isOpen) {
			resourceManagerConfigFile.read((char*)&recordedLastTimeFbxWasEdited, sizeof(recordedLastTimeFbxWasEdited));
		}
		else {
			logError("Resource manager config file could not be loaded!");
		}
	}

	if (lastTimeFbxWasEdited > recordedLastTimeFbxWasEdited) {
		for (auto filePath : paths) {
			if (lastWrittenTime(filePath) > recordedLastTimeFbxWasEdited) {
				Scene scene;
				scene.loadFBX(filePath, *this);
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

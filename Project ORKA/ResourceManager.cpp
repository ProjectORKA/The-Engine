
#include "ResourceManager.hpp"
#include "Scene.hpp"



ResourceManager::ResourceManager(FileSystem& fileSystem) {
	//the "ORKA" location is the folder that contains the executable
	orkaLocation = makeAbsolute(fileSystem.executablePath);
	
	//in in we will always have a "Data" and "Cache" folder
	//"Data" contains all resources like meshes, shaders, levels, textures, scrips, etc
	orkaDataLocation = orkaLocation;
	orkaDataLocation.append("Data");
	//"Cache" contains data that ORKA will create itself, so things like uncompressed or encrypted files, configs, logs, temporary data, etc
	orkaCacheLocation = orkaLocation;
	orkaCacheLocation.append("Cache");

	orkaCachedMeshLocation = orkaCacheLocation;
	orkaCachedMeshLocation.append("Meshes");

	//setup data folder
	if (!doesPathExist(orkaDataLocation))createDirectory(orkaDataLocation);
	//setup cache folder
	if (!doesPathExist(orkaCacheLocation))createDirectory(orkaCacheLocation);

	reloadAllResources(fileSystem);
}
void ResourceManager::reloadAllResources(FileSystem & fileSystem) {

	loadResourcesFromFBXFiles(fileSystem);

	Vector<Path> paths;

	//load all supported files
	paths.clear();
	paths = getAllFilesInDirectory(orkaLocation, { ".mesh", ".png", ".bmp", ".exr", ".hdr", ".jpg", ".vert", ".frag" });
	
	for (auto p : paths) {
		String extension = p.extension().string();
		
		if (extension == ".mesh") {
			meshResources[Name(p.stem().string())] = p;
			continue;
		}

		if (extension == ".png" || extension == ".bmp" || extension == ".exr" || extension == ".hdr" || extension == ".jpg") {
			textureResources[Name(p.stem().string())] = p;
			continue;
		}

		if (extension == ".vert") {
			vertexShaderResources[Name(p.stem().string())] = p;
			continue;
		}

		if (extension == ".frag") {
			fragmentShaderResources[Name(p.stem().string())] = p;
			continue;
		}
	}

}
void ResourceManager::loadResourcesFromFBXFiles(FileSystem& fileSystem) {

	//get all paths to fbx files
	Vector<Path> paths;
	paths = getAllFilesInDirectory(orkaDataLocation, { ".fbx" });

	//get the last edited times and the stored value
	FileTime lastTimeFbxWasEdited = getLastWrittenTimeOfFiles(paths);
	Path resourceManagerConfigPath = makeAbsolute(Path(orkaCacheLocation).append("config").append("resourceManager.bin"));

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

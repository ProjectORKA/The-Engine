
#include "ResourceManager.hpp"
#include "Scene.hpp"

void ResourceManager::init() {
	//make sure the resource manager is only initialized once
	if (initialized) logError("Resource manager already initialized!");
	else {
		//the "ORKA" location is the folder that contains the executable
		orkaLocation = makeAbsolute(executablePath);

		//in in we will always have a "Data" and "Cache" folder
		//"Data" contains all resources like meshes, shaders, levels, textures, scrips, etc
		orkaDataLocation = orkaLocation;
		orkaDataLocation.append("Data");
		//"Cache" contains data that ORKA will create itself, so things like uncompressed or encrypted files, configs, logs, temporary data, etc
		orkaCacheLocation = orkaLocation;
		orkaCacheLocation.append("Cache");
		//meshes are uncompressed 3d objects that can be loaded straight into memory
		orkaCachedMeshLocation = orkaCacheLocation;
		orkaCachedMeshLocation.append("Meshes");
		//the resource manager config keeps track of file updates
		resourceManagerConfigPath = makeAbsolute(Path(orkaCacheLocation).append("config").append("resourceManager.bin"));

		//setup data folder
		if (!doesPathExist(orkaDataLocation))createDirectory(orkaDataLocation);
		//setup cache folder
		if (!doesPathExist(orkaCacheLocation))createDirectory(orkaCacheLocation);

		initialized = true;

		reloadAllResources();
	}
}
void ResourceManager::reloadAllResources() {

	loadResourcesFromFBXFiles();

	Vector<Path> paths;

	//get all data files
	paths.clear();
	paths = getAllFilesInDirectory(orkaDataLocation, { ".mesh", ".png", ".bmp", ".exr", ".hdr", ".jpg", ".vert", ".frag" });
	for (auto p : paths) addResource(p);

	//get all cached files
	paths.clear();
	paths = getAllFilesInDirectory(orkaCacheLocation, { ".mesh", ".png", ".bmp", ".exr", ".hdr", ".jpg", ".vert", ".frag" });
	for (auto p : paths) addResource(p);
}
void ResourceManager::addResource(Path path) {
	String extension = path.extension().string();
	if (extension == ".mesh") {
		meshResources[Name(path.stem().string())] = path;
		return;
	}
	if (extension == ".png" || extension == ".bmp" || extension == ".exr" || extension == ".hdr" || extension == ".jpg") {
		textureResources[Name(path.stem().string())] = path;
		return;
	}
	if (extension == ".vert") {
		vertexShaderResources[Name(path.stem().string())] = path;
		return;
	}
	if (extension == ".frag") {
		fragmentShaderResources[Name(path.stem().string())] = path;
		return;
	}
}
void ResourceManager::loadResourcesFromFBXFiles() {

	//get all paths to fbx files
	Vector<Path> paths;
	paths = getAllFilesInDirectory(orkaDataLocation, { ".fbx" });

	//get the last edited times and the stored value
	FileTime lastTimeFbxWasEdited = getLastWrittenTimeOfFiles(paths);

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

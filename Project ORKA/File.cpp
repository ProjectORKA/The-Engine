#include "File.hpp"
#include "FileSystem.hpp"

InFile::~InFile() {
	file.close();
}

InFile::InFile(Path location) {

	fileLocation = makeAbsolute(location);

	file = std::ifstream(location, std::ios::binary);
	if (file.is_open()) {
		isOpen = true;
	}
	else {
		isOpen = false;
		logWarning(String("Could not open file at location (").append(fileLocation.string()).append(")!"));
	}
}


void InFile::read(char* data, SizeT size) {
	if (isOpen) {
		file.read(data, size);
	}
	else {
		logError(String("The binary file").append(fileLocation.string()).append("could not be opened! Dont try to read it!"));
	}
}

OutFile::~OutFile() {
	file.close();
}

OutFile::OutFile(Path location) {

	fileLocation = makeAbsolute(location);

	Path parentPath = fileLocation.parent_path();

	while (!doesPathExist(parentPath)) {

		logDebug(String("The directory (").append(parentPath.string()).append(") does not exist and will be created!"));
		createDirectory(parentPath);
	}

	file = std::ofstream(fileLocation, std::ios::trunc | std::ios::binary | std::ios::out);
	if (file.is_open()) {
		isOpen = true;
	}
	else {
		isOpen = false;
		logError(String("Could not create file at location (").append(fileLocation.string()).append(")!"));
	}
}

void OutFile::write(char* data, SizeT size) {
	if (isOpen) {
		file.write(data, size);
	}
	else {
		logError(String("The binary file").append(fileLocation.string()).append("could not be opened!"));
	}
}
#include "File.hpp"
#include "FileSystem.hpp"

InFile::~InFile() {
	file.close();
}
InFile::InFile(Path location) {
	if (doesPathExist(location)) {
		fileLocation = location;
		file = std::ifstream(fileLocation, std::ios::binary | std::ios::in);
		if (file.is_open()) {
			isOpen = true;
		}
	}
	else {
		isOpen = false;
		logDebug(String("Could not create file at location (").append(fileLocation.string()).append(")!"));
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

	fileLocation = location;
	while (!std::filesystem::exists(fileLocation.parent_path())) {
		logDebug(String("The directory (").append(fileLocation.parent_path().string()).append(") does not exist and will be created!"));
		std::filesystem::create_directory(fileLocation.parent_path());
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


#include "Basics.hpp"

struct InFile {
	Path fileLocation;
	Bool isOpen = false;
	std::ifstream file;

	InFile(Path location) {
		fileLocation = location;
		file = std::ifstream(fileLocation, std::ios::binary | std::ios::in);
		if (file.is_open()) {
			isOpen = true;
		}
		else {
			isOpen = false;
			logError(String("Could not create file at location (").append(fileLocation.string()).append(")!"));
		}
	}

	void read(char * data, SizeT size) {
		if (isOpen) {
			file.read(data, size);
		}
		else {
			logError(String("The binary file").append(fileLocation.string()).append("could not be opened!"));
		}
	}

	~InFile() {
		auto pos1 = file.tellg();
		file.eof();
		auto pos2 = file.tellg();
		if (pos1 == pos2) logDebug("File loaded successfully!");
		else logError("File was loaded, but did not exit at end!");
		file.close();
	}
};

struct OutFile {
	Path fileLocation;
	Bool isOpen = false;
	std::ofstream file;

	OutFile(Path location) {
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

	void write(char* data, SizeT size) {
		if (isOpen) {
			file.write(data, size);
		}
		else {
			logError(String("The binary file").append(fileLocation.string()).append("could not be opened!"));
		}
	}

	~OutFile() {
		file.close();
	}
};
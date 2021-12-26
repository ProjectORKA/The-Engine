
#include "Sortr.hpp"

void removeDoubleQuotes(String& s) {
	s.erase(std::remove(s.begin(), s.end(), '\"'), s.end());
}

void sortr() {
	Path unsortedFolder;
	Vector<Path> sortFolders;

	std::cout << "Unsorted folder: " << std::endl;

	String unsortedFilePathString;

	std::getline(std::cin, unsortedFilePathString);

	removeDoubleQuotes(unsortedFilePathString);

	unsortedFolder = Path(unsortedFilePathString);

	if (std::filesystem::exists(unsortedFolder)) {
		for (auto& d : std::filesystem::directory_iterator(unsortedFolder))
		{
			Path filePath = d.path();

			if (filePath.filename().extension() == ".png") {
				std::cout << filePath.string() << '\n';
			}
		}
	}
	else {
		logDebug("Not a valid directory!");
	}

	logDebug("All files sorted!");
}

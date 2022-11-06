
#pragma once

#include "Basics.hpp"
#include "Debug.hpp"
#include "Image.hpp"
#include "File.hpp"

using FileTime = std::filesystem::file_time_type;


Path makeAbsolute(Path path);
Bool doesPathExist(Path path);
FileTime lastWrittenTime(Path path);
Path nameToPath(Name name, String filetype);
void copyFile(Path source, Path destination);
Vector<Path> getAllPathsInDirectory(Path path);
Vector<Path> getAllFilesInDirectory(Path path);
FileTime getLastWrittenTimeOfFiles(Vector<Path> paths);
Image loadImage(Path path, Int bitcount, Bool inverted);
Vector<Path> getAllFilesInDirectory(Path path, Vector<String> filter);


struct FileSystem {
	Path executablePath = "";

	void init(String path) {
		executablePath = path;
	}

	String loadString(Path path);
};

extern FileSystem fileSystem;
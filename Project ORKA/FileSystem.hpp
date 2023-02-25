
#pragma once

#include "Basics.hpp"
#include "Debug.hpp"
#include "Image.hpp"
#include "File.hpp"

using FileTime = std::filesystem::file_time_type;

String loadString(Path path);
Path makeAbsolute(Path path);
Path getDirectory(Path path);
Bool doesPathExist(Path path);
Path removeFileName(Path path);
void createDirectory(Path path);
FileTime lastWrittenTime(Path path);
Path nameToPath(Name name, String filetype);
void copyFile(Path source, Path destination);
Vector<Path> getAllPathsInDirectory(Path path);
Vector<Path> getAllFilesInDirectory(Path path);
FileTime getLastWrittenTimeOfFiles(Vector<Path> paths);
Image loadImage(Path path, Int bitcount, Bool inverted);
Vector<Path> getAllFilesInDirectory(Path path, Vector<String> filter);

extern Path executablePath;

struct FileSystem {
	FileSystem();
};
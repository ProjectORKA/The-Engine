#pragma once

#include "Basics.hpp"
#include "Debug.hpp"
#include "Image.hpp"
#include "File.hpp"

using FileTime = std::filesystem::file_time_type;

void deleteFile(const Path& path);
UInt getFileSize(const Path& path);
String loadString(const Path& path);
Path getDirectory(Path path);
Path makeAbsolute(const Path& path);
Bool doesPathExist(const Path& path);
String getFileName(const Path& path);
Path removeFileName(Path path);
void createDirectory(Path path);
FileTime lastWrittenTime(const Path& path);
Vector<String> loadStringVector(const Path& path);
Vector<Path> getFilesInDirectory(const Path& path);
Path nameToPath(const Name& name, const String& filetype);
void copyFile(const Path& source, const Path& destination);
Vector<Path> getAllPathsInDirectory(const Path& path);
Vector<Path> getAllFilesInDirectory(const Path& path);
FileTime getLastWrittenTimeOfFiles(const Vector<Path>& paths);
Vector<Path> getFilesInDirectory(const Path& path, const Vector<String>& filter);
Vector<Path> getAllFilesInDirectory(const Path& path, const Vector<String>& filter);

extern Path executablePath;

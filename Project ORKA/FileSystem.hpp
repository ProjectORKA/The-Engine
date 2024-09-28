#pragma once

#include "Basics.hpp"

using FileTime = std::filesystem::file_time_type;

void createDirectory(Path path);
void deleteFile(const Path& path);
void setCurrentPath(const Path& path);
void copyFile(const Path& source, const Path& destination);
void moveFile(const Path& source, const Path& destination);

[[nodiscard]] ULL getFileSize(const Path& path);

[[nodiscard]] String loadString(const Path& path);
[[nodiscard]] String getFileName(const Path& path);
[[nodiscard]] String getFolderName(const Path& path);

[[nodiscard]] Path getCurrentPath();
[[nodiscard]] Path getDirectory(Path path);
[[nodiscard]] Path removeFileName(Path path);
[[nodiscard]] Path makeAbsolute(const Path& path);
[[nodiscard]] Path getParentFolder(const Path& path);

[[nodiscard]] Bool hasExtension(const Path& path);
[[nodiscard]] Bool doesPathExist(const Path& path);
[[nodiscard]] Bool doesFileExist(const Path& path);

[[nodiscard]] FileTime getLastWrittenTime(const Path& path);
[[nodiscard]] FileTime getLastWrittenTimeOfFiles(const Vector<Path>& paths);

[[nodiscard]] Vector<String> loadStringVector(const Path& path);

[[nodiscard]] Vector<Path> getFilesInDirectory(const Path& path);
[[nodiscard]] Vector<Path> getAllFilesInDirectory(const Path& path);
[[nodiscard]] Vector<Path> getAllPathsInDirectory(const Path& path);
[[nodiscard]] Vector<Path> getFilesInDirectory(const Path& path, const Vector<String>& filter);
[[nodiscard]] Vector<Path> getAllFilesInDirectory(const Path& path, const Vector<String>& filter);

extern Path executablePath;

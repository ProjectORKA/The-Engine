#include "FileSystem.hpp"
#include "Debug.hpp"
#include "File.hpp"
#include "FileTypes.hpp"
#include "Windows.hpp"

Path executablePath = "";

Path getCurrentPath()
{
	return std::filesystem::current_path();
}

Path getDirectory(Path path)
{
	path = makeAbsolute(path);
	path = removeFileName(path);
	return path;
}

Path removeFileName(Path path)
{
	if (hasExtension(path)) return path.remove_filename();
	return path;
}

void createDirectory(Path path)
{
	path = removeFileName(path);
	create_directory(path);
}

void deleteFile(const Path& path)
{
	// if operating system is windows
	if constexpr (_WIN32)
	{
		// send to windows recycle bin
		SHFILEOPSTRUCT fileOp;
		ZeroMemory(&fileOp, sizeof fileOp);
		fileOp.hwnd    = nullptr;
		fileOp.wFunc   = FO_DELETE;
		const String s = path.string();
		fileOp.pFrom   = s.c_str();
		fileOp.fFlags  = FOF_ALLOWUNDO | FOF_NOCONFIRMATION;

		const Int result = SHFileOperation(&fileOp);
		if (result != 0) logError("Could not delete file!");
	}
	else
	{
		std::filesystem::remove(path);
	}
}

ULL getFileSize(const Path& path)
{
	return file_size(path);
}

String loadString(const Path& path)
{
	if (std::ifstream stream(path, std::ios::in); stream.is_open())
	{
		std::stringstream stringStream;
		stringStream << stream.rdbuf();
		String s = stringStream.str();
		stream.close();
		return s;
	}
	logError("String Cant be read! (" + path.string() + ")");
	return "";
}

Path makeAbsolute(const Path& path)
{
	return absolute(path);
}

Bool hasExtension(const Path& path)
{
	return path.has_extension();
}

Bool doesPathExist(const Path& path)
{
	return exists(makeAbsolute(path));
}

Bool doesFileExist(const Path& path)
{
	return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

String getFileName(const Path& path)
{
	if (path.has_filename())
	{
		// there seems to be an issue with, for example japanese characters, that make this conversion necessary
		const WString                                    wideString = path.filename().wstring();
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		return converter.to_bytes(wideString);
	}
	logError("Could not resolve filename!");
	return "error";
}

String getFolderName(const Path& path)
{
	if (is_directory(path))
	{
		if (path.string().back() == '\\' || path.string().back() == '/')
		{
			return path.parent_path().filename().string();
		}
		return path.filename().string();
	}
	logError("Path is not folder!");
	return "";
}

void setCurrentPath(const Path& path)
{
	std::filesystem::current_path(path);
}

Path getParentFolder(const Path& path)
{
	return path.parent_path();
}

FileTime getLastWrittenTime(const Path& path)
{
	return last_write_time(path);
}

Vector<String> loadStringVector(const Path& path)
{
	Vector<String> lines;
	InFile         file(path);
	String         line;
	while (std::getline(file.file, line))
	{
		std::erase(line, '\r');
		lines.push_back(line);
	}
	return lines;
}

Vector<Path> getFilesInDirectory(const Path& path)
{
	Vector<Path> paths;
	for (const auto& file : std::filesystem::directory_iterator(path)) if (file.is_regular_file()) paths.push_back(file.path());
	return paths;
}

Vector<Path> getAllPathsInDirectory(const Path& path)
{
	Vector<Path> paths;
	for (const auto& file : std::filesystem::recursive_directory_iterator(path)) paths.push_back(file.path());
	return paths;
}

Vector<Path> getAllFilesInDirectory(const Path& path)
{
	Vector<Path> paths;
	for (const auto& file : std::filesystem::recursive_directory_iterator(path)) if (file.is_regular_file()) paths.push_back(file.path());
	return paths;
}

void copyFile(const Path& source, const Path& destination)
{
	logDebug("Copying file (" + source.string() + ") to (" + destination.string() + ")");

	const Path& sourceFile   = source;
	const Path& targetParent = destination;
	const auto  target       = targetParent / sourceFile.filename();

	try
	{
		create_directories(targetParent);
		copy_file(sourceFile, target, std::filesystem::copy_options::overwrite_existing);
	}
	catch (std::exception& e)
	{
		std::cout << e.what();
	}
}

void moveFile(const Path& source, const Path& destination)
{
	copyFile(source, destination);
	const UInt   sourceFileSize = getFileSize(source);
	const String fileName       = getFileName(source);
	const Path   destFile       = destination.string() + fileName;
	const UInt   destFileSize   = getFileSize(destFile);
	logDebug(destFile);
	logDebug(sourceFileSize);
	logDebug(destFileSize);

	//comp sizes
	if (const Bool fileValid = doesPathExist(destination); fileValid && sourceFileSize == destFileSize)
	{
		deleteFile(source);
	}
	else
	{
		logError("File size corrupted during moving, doesn't match!");
	}
}

FileTime getLastWrittenTimeOfFiles(const Vector<Path>& paths)
{
	FileTime t1;
	for (const auto& p : paths)
	{
		if (FileTime t2 = getLastWrittenTime(p); t2 > t1) t1 = t2;
	}
	return t1;
}

Vector<Path> getFilesInDirectory(const Path& path, const Vector<String>& filter)
{
	Vector<Path>       outputFiles;
	const Vector<Path> filesInDirectory = getFilesInDirectory(path);
	for (const Path& filePath : filesInDirectory) for (const String& extension : filter) if (getFileExtension(filePath) == extension) outputFiles.push_back(filePath);
	return outputFiles;
}

Vector<Path> getAllFilesInDirectory(const Path& path, const Vector<String>& filter)
{
	Vector<Path>       outputFiles;
	const Vector<Path> filesInDirectory = getAllFilesInDirectory(path);
	for (const Path& filePath : filesInDirectory) for (const String& extension : filter) if (getFileExtension(filePath) == extension) outputFiles.push_back(filePath);
	return outputFiles;
}

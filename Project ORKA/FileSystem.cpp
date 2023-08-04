#include "FileSystem.hpp"
#include "Debug.hpp"
#include "File.hpp"
#include "Platform.hpp"

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
	if(path.has_extension()) return path.remove_filename();
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
	if constexpr(_WIN32)
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
		if(result != 0) logError("Could not delete file!");
	}
	else
	{
		std::filesystem::remove(path);
	}
}

UInt getFileSize(const Path& path)
{
	return file_size(path);
}

String loadString(const Path& path)
{
	String        s;
	std::ifstream stream(path, std::ios::in);
	if(stream.is_open())
	{
		std::stringstream sstr;
		sstr << stream.rdbuf();
		s = sstr.str();
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
	if(path.has_filename()) return path.filename().string();
	logError("Could not resolve filename!");
}

void setCurrentPath(const Path& path)
{
	std::filesystem::current_path(path);
}

FileTime lastWrittenTime(const Path& path)
{
	return last_write_time(path);
}

Vector<String> loadStringVector(const Path& path)
{
	Vector<String> lines;

	InFile file(path);

	String line;
	while(std::getline(file.file, line)) lines.push_back(line);

	return lines;
}

Vector<Path> getFilesInDirectory(const Path& path)
{
	Vector<Path> paths;
	for(const auto& file : std::filesystem::directory_iterator(path)) if(file.is_regular_file()) paths.push_back(file.path());
	return paths;
}

Vector<Path> getAllPathsInDirectory(const Path& path)
{
	Vector<Path> paths;
	for(const auto& file : std::filesystem::recursive_directory_iterator(path)) paths.push_back(file.path());
	return paths;
}

Vector<Path> getAllFilesInDirectory(const Path& path)
{
	Vector<Path> paths;
	for(const auto& file : std::filesystem::recursive_directory_iterator(path)) if(file.is_regular_file()) paths.push_back(file.path());
	return paths;
}

Path nameToPath(const Name& name, const String& fileType)
{
	if(fileType == ".fbx") return "Data/objects/" + toString(name) + ".fbx";
	if(fileType == ".mesh") return "Data/meshes/" + toString(name) + ".mesh";
	logError("Filetype not supported!");
}

void copyFile(const Path& source, const Path& destination)
{
	logDebug("Copying file (" + source.string() + ") to (" + destination.string() + ")");

	const Path sourceFile   = source;
	const Path targetParent = destination;
	const auto target       = targetParent / sourceFile.filename();

	try
	{
		create_directories(targetParent);
		copy_file(sourceFile, target, std::filesystem::copy_options::overwrite_existing);
	}
	catch(std::exception& e)
	{
		std::cout << e.what();
	}
}

FileTime getLastWrittenTimeOfFiles(const Vector<Path>& paths)
{
	FileTime t1;

	for(auto p : paths)
	{
		FileTime t2 = lastWrittenTime(p);
		if(t2 > t1) t1 = t2;
	}

	return t1;
}

Vector<Path> getFilesInDirectory(const Path& path, const Vector<String>& filter)
{
	Vector<Path> paths1;
	Vector<Path> paths2;

	paths1 = getFilesInDirectory(path);

	for(auto p : paths1) for(auto f : filter) if(p.extension() == f) paths2.push_back(p);

	return paths2;
}

Vector<Path> getAllFilesInDirectory(const Path& path, const Vector<String>& filter)
{
	Vector<Path> paths1;
	Vector<Path> paths2;

	paths1 = getAllFilesInDirectory(path);

	for(auto p : paths1) for(auto f : filter) if(p.extension() == f) paths2.push_back(p);

	return paths2;
}


#include "FileSystem.hpp"
#include "Platform.hpp"

Path executablePath = "";

void deleteFile(Path path) {
	//if operating system is windows
	if (_WIN32) {
		//send to windows recycle bin
		SHFILEOPSTRUCT fileOp;
		ZeroMemory(&fileOp, sizeof(fileOp));
		fileOp.hwnd = NULL;
		fileOp.wFunc = FO_DELETE;
		String s = path.string();
		fileOp.pFrom = s.c_str();
		fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION;

		Int result = SHFileOperation(&fileOp);
		if (result != 0) {
			logError("Could not delete file!");
		}
	}
	else {
		std::filesystem::remove(path);
	}
}
String loadString(Path path) {
	String s;
	std::ifstream stream(path, std::ios::in);
	if (stream.is_open()) {
		std::stringstream sstr;
		sstr << stream.rdbuf();
		s = sstr.str();
		stream.close();
		return s;
	}
	else {
		logError(String("String Cant be read! (").append(path.string()).append(")"));
		return "";
	}
}
Path getDirectory(Path path) {
	path = makeAbsolute(path);
	path = removeFileName(path);
	return path;
}
Path makeAbsolute(Path path) {
	return std::filesystem::absolute(path);
}
Bool doesPathExist(Path path)
{
	return std::filesystem::exists(makeAbsolute(path));
}
String getFileName(Path path) {
	if (path.has_filename())
		return path.filename().string();
	else logError("Could not resolve filename!");
}
Path removeFileName(Path path) {
	if (path.has_extension())
		return path.remove_filename();
	else return path;
}
void createDirectory(Path path) {
	path = removeFileName(path);
	std::filesystem::create_directory(path);
}
FileTime lastWrittenTime(Path path) {
	return std::filesystem::last_write_time(path);
}
Vector<String> loadStringVector(Path path)
{
	Vector<String> lines;

	InFile file(path);
	
	String line;
	while (std::getline(file.file, line)) {
		lines.pushBack(line);
	}

	return lines;
}
Vector<Path> getFilesInDirectory(Path path) {
	Vector<Path> paths;
	for (const auto& file : std::filesystem::directory_iterator(path)) {
		if (file.is_regular_file()) paths.pushBack(file.path());
	}
	return paths;
}
Path nameToPath(Name name, String filetype) {
	if (filetype == ".fbx") return Path(String("data/objects/").append(name.data).append(".fbx"));
	if (filetype == ".mesh") return Path(String("data/meshes/").append(name.data).append(".mesh"));
	logError("Filetype not supported!");
}
void copyFile(Path source, Path destination) {
	logDebug(String("Copying file (").append(source.string()).append(") to (").append(destination.string()).append(")"));

	Path sourceFile = source;
	Path targetParent = destination;
	auto target = targetParent / sourceFile.filename();

	try
	{
		std::filesystem::create_directories(targetParent);
		std::filesystem::copy_file(sourceFile, target, std::filesystem::copy_options::overwrite_existing);
	}
	catch (std::exception& e)
	{
		std::cout << e.what();
	}
}
Vector<Path> getAllPathsInDirectory(Path path) {
	Vector<Path> paths;
	for (const auto& file : std::filesystem::recursive_directory_iterator(path)) {
		paths.pushBack(file.path());
	}
	return paths;
}
Vector<Path> getAllFilesInDirectory(Path path) {
	Vector<Path> paths;
	for (const auto& file : std::filesystem::recursive_directory_iterator(path)) {
		if (file.is_regular_file()) paths.pushBack(file.path());
	}
	return paths;
}
FileTime getLastWrittenTimeOfFiles(Vector<Path> paths) {

	FileTime t1;

	for (auto p : paths) {
		FileTime t2 = lastWrittenTime(p);
		if (t2 > t1) t1 = t2;
	}

	return t1;
}
Vector<Path> getFilesInDirectory(Path path, Vector<String> filter) {
	Vector<Path> paths1;
	Vector<Path> paths2;

	paths1 = getFilesInDirectory(path);

	for (auto p : paths1) {
		for (auto f : filter) {
			if (p.extension() == f)paths2.pushBack(p);
		}
	}

	return paths2;
}
Vector<Path> getAllFilesInDirectory(Path path, Vector<String> filter) {
	Vector<Path> paths1;
	Vector<Path> paths2;

	paths1 = getAllFilesInDirectory(path);
	
	for (auto p : paths1) {
		for (auto f : filter) {
			if (p.extension() == f)paths2.pushBack(p);
		}
	}

	return paths2;
}

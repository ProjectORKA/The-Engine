
#include "FileSystem.hpp"

FileSystem fileSystem;

namespace stbi {
	//#define STBI_NO_JPEG
#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
}

void FileSystem::init(String path) {
	executablePath = path;
}
String FileSystem::loadString(Path path) {
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
		logWarning(String("String Cant be read! (").append(path.string()).append(")"));
		return "";
	}
}
Path FileSystem::makeAbsolute(Path path) {
	return std::filesystem::absolute(path);
}
Bool FileSystem::doesPathExist(Path path) {
	return std::filesystem::exists(path);
}
FileTime FileSystem::lastWrittenTime(Path path) {
	return std::filesystem::last_write_time(path);
}
Path FileSystem::nameToPath(Name name, String filetype) {
	if (filetype == ".fbx") return Path(String("data/objects/").append(name.data).append(".fbx"));
	if (filetype == ".mesh") return Path(String("data/meshes/").append(name.data).append(".mesh"));
	logError("Filetype not supported!");
}
void FileSystem::copyFile(Path source, Path destination) {
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
Vector<Path> FileSystem::getAllPathsInDirectory(Path path) {
	Vector<Path> paths;
	for (const auto& file : std::filesystem::recursive_directory_iterator(path)) {
		paths.push_back(file.path());
	}
	return paths;
}
Vector<Path> FileSystem::getAllFilesInDirectory(Path path) {
	Vector<Path> paths;
	for (const auto& file : std::filesystem::recursive_directory_iterator(path)) {
		if (file.is_regular_file()) paths.push_back(file.path());
	}
	return paths;
}
FileTime FileSystem::getLastWrittenTimeOfFiles(Vector<Path> paths) {

	FileTime t1;

	for (auto p : paths) {
		FileTime t2 = lastWrittenTime(p);
		if (t2 > t1) t1 = t2;
	}

	return t1;
}
Image FileSystem::loadImage(Path path, Int bitcount, Bool inverted) {
	stbi::stbi_set_flip_vertically_on_load(inverted);

	Image image;
	if (bitcount == 8) {
		image.pixels = stbi::stbi_load(path.string().c_str(), &image.width, &image.height, &image.channels, 0);
	}
	else {
		if (bitcount == 16) {
			image.pixels = reinterpret_cast <Byte*>(stbi::stbi_load_16(path.string().c_str(), &image.width, &image.height, &image.channels, 0));
			image.bitcount = 16;
		}
		else {
			logError("Bitcount not supported!");
		}
	}

	if (!image.pixels)logWarning("Image not loaded!");

	return image;
}
Vector<Path> FileSystem::getAllFilesInDirectory(Path path, Vector<String> filter) {
	Vector<Path> paths;
	for (const auto& file : std::filesystem::recursive_directory_iterator(path)) {
		Bool use = file.is_regular_file();

		for (String& fileType : filter) {

			if (file.path().extension() == fileType) {
				paths.push_back(file.path());
				break;
			}
		}
	}
	return paths;
}
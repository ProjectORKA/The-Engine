#pragma once

#include "Basics.hpp"
#include "FileSystem.hpp"

enum class FileType {
	Application,
	Mesh,
	Image,
	Shader,
	Font,
	Audio,
	Binary,
	Unknown
};

extern Vector<String> executableExtensions;
extern Vector<String> meshExtensions;
extern Vector<String> imageExtensions;
extern Vector<String> fontExtensions;
extern Vector<String> audioExtensions;
extern Vector<String> binaryExtensions;
extern Vector<String> shaderExtensions;

inline String getFileExtension(const Path& path) { return path.extension().string(); }

inline Bool isExecutableFile(const Path& path) {
	const String extension = getFileExtension(path);
	for (String ext : executableExtensions) { if (extension == ext) return true; }
	return false;
}

inline Bool isImageFile(const Path& path) {
	const String extension = getFileExtension(path);
	for (String ext : imageExtensions) { if (extension == ext) return true; }
	return false;
}

inline Bool isMeshFile(const Path& path) {
	const String extension = getFileExtension(path);
	for (String ext : meshExtensions) { if (extension == ext) return true; }
	return false;
}

inline Bool isShaderFile(const Path& path) {
	const String extension = getFileExtension(path);
	for (String ext : shaderExtensions) { if (extension == ext) return true; }
	return false;
}

inline Bool isFontFile(const Path& path) {
	const String extension = getFileExtension(path);
	for (String ext : fontExtensions) { if (extension == ext) return true; }
	return false;
}

inline Bool isAudioFile(const Path& path) {
	const String extension = getFileExtension(path);
	for (String ext : audioExtensions) { if (extension == ext) return true; }
	return false;
}

inline Bool isBinaryFile(const Path& path) {
	const String extension = getFileExtension(path);
	for (String ext : binaryExtensions) { if (extension == ext) return true; }
	return false;
}

inline FileType getFileType(const Path& path) {
	if (isExecutableFile(path)) return FileType::Application;
	if (isImageFile(path)) return FileType::Image;
	if (isMeshFile(path)) return FileType::Mesh;
	if (isShaderFile(path)) return FileType::Shader;
	if (isFontFile(path)) return FileType::Font;
	if (isAudioFile(path)) return FileType::Audio;
	if (isBinaryFile(path)) return FileType::Binary;
	return FileType::Unknown;
}

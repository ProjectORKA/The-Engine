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

const String executableExtensions[] = { ".exe" };
const String meshExtensions[] = { ".mesh", ".obj", ".fbx", ".blend" };
const String imageExtensions[] = { ".png", ".jpg", ".jpeg", ".bmp", ".tga" };
const String shaderExtensions[] = { ".vert", ".frag", ".geom", ".comp", ".tess", ".glsl" };
const String fontExtensions[] = { ".ttf" };
const String audioExtensions[] = { ".ogg", ".wav", ".mp3" };
const String binaryExtensions[] = { ".bin" };

String getFileExtension(Path path) {
	return path.extension().string();
}
Bool isExecutableFile(Path path) {
	String extension = getFileExtension(path);
	for (String ext : executableExtensions) {
		if (extension == ext) return true;
	}
	return false;
}
Bool isImageFile(Path path) {
	String extension = getFileExtension(path);
	for (String ext : imageExtensions) {
		if (extension == ext) return true;
	}
	return false;
}
Bool isMeshFile(Path path) {
	String extension = getFileExtension(path);
	for (String ext : meshExtensions) {
		if (extension == ext) return true;
	}
	return false;
}
Bool isShaderFile(Path path) {
	String extension = getFileExtension(path);
	for (String ext : shaderExtensions) {
		if (extension == ext) return true;
	}
	return false;
}
Bool isFontFile(Path path) {
	String extension = getFileExtension(path);
	for (String ext : fontExtensions) {
		if (extension == ext) return true;
	}
	return false;
}
Bool isAudioFile(Path path) {
	String extension = getFileExtension(path);
	for (String ext : audioExtensions) {
		if (extension == ext) return true;
	}
	return false;
}
Bool isBinaryFile(Path path) {
	String extension = getFileExtension(path);
	for (String ext : binaryExtensions) {
		if (extension == ext) return true;
	}
	return false;
}
FileType getFileType(Path path) {
	if (isExecutableFile(path)) return FileType::Application;
	if (isImageFile(path)) return FileType::Image;
	if (isMeshFile(path)) return FileType::Mesh;
	if (isShaderFile(path)) return FileType::Shader;
	if (isFontFile(path)) return FileType::Font;
	if (isAudioFile(path)) return FileType::Audio;
	if (isBinaryFile(path)) return FileType::Binary;
	return FileType::Unknown;
}

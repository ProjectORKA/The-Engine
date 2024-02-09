#include "FileTypes.hpp"

Vector<String> executableExtensions = {".exe"};
Vector<String> fontExtensions       = {".ttf"};
Vector<String> binaryExtensions     = {".bin"};
Vector<String> audioExtensions      = {".ogg", ".wav", ".mp3"};
Vector<String> meshExtensions       = {".mesh", ".obj", ".fbx", ".blend"};
Vector<String> shaderExtensions     = {".vert", ".frag", ".geom", ".comp", ".tess", ".glsl"};
Vector<String> imageExtensions      = {".png", ".jpg", ".jpeg", ".bmp", ".tga", ".webp", ".jfif"};

Bool isMeshFile(const Path& path)
{
	const String extension = getFileExtension(path);
	for(const String& ext : meshExtensions) if(extension == ext) return true;
	return false;
}

Bool isFontFile(const Path& path)
{
	const String extension = getFileExtension(path);
	for(const String& ext : fontExtensions) if(extension == ext) return true;
	return false;
}

Bool isImageFile(const Path& path)
{
	const String extension = getFileExtension(path);
	for(const String& ext : imageExtensions) if(extension == ext) return true;
	return false;
}

Bool isAudioFile(const Path& path)
{
	const String extension = getFileExtension(path);
	for(const String& ext : audioExtensions) if(extension == ext) return true;
	return false;
}

Bool isShaderFile(const Path& path)
{
	const String extension = getFileExtension(path);
	for(const String& ext : shaderExtensions) if(extension == ext) return true;
	return false;
}

Bool isBinaryFile(const Path& path)
{
	const String extension = getFileExtension(path);
	for(const String& ext : binaryExtensions) if(extension == ext) return true;
	return false;
}

FileType getFileType(const Path& path)
{
	if(isExecutableFile(path)) return FileType::Application;
	if(isImageFile(path)) return FileType::Image;
	if(isMeshFile(path)) return FileType::Mesh;
	if(isShaderFile(path)) return FileType::Shader;
	if(isFontFile(path)) return FileType::Font;
	if(isAudioFile(path)) return FileType::Audio;
	if(isBinaryFile(path)) return FileType::Binary;
	return FileType::Unknown;
}

Bool isExecutableFile(const Path& path)
{
	const String extension = getFileExtension(path);
	for(const String& ext : executableExtensions) if(extension == ext) return true;
	return false;
}

String getFileExtension(const Path& path)
{
	return toLowerCase(path.extension().string());
}

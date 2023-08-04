#pragma once

#include "Basics.hpp"

enum class FileType
{
	Application,
	Mesh,
	Image,
	Shader,
	Font,
	Audio,
	Binary,
	Unknown
};

extern Vector<String> fontExtensions;
extern Vector<String> meshExtensions;
extern Vector<String> audioExtensions;
extern Vector<String> imageExtensions;
extern Vector<String> shaderExtensions;
extern Vector<String> binaryExtensions;
extern Vector<String> executableExtensions;

Bool     isMeshFile(const Path& path);
Bool     isFontFile(const Path& path);
Bool     isImageFile(const Path& path);
Bool     isAudioFile(const Path& path);
FileType getFileType(const Path& path);
Bool     isBinaryFile(const Path& path);
Bool     isShaderFile(const Path& path);
Bool     isExecutableFile(const Path& path);
String   getFileExtension(const Path& path);

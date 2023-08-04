#include "File.hpp"
#include "Debug.hpp"
#include "FileSystem.hpp"

InFile::~InFile()
{
	file.close();
}

OutFile::~OutFile()
{
	file.close();
}

ULL InFile::fileSize()
{
	ULL size = 0;
	file.seekg(0, std::ios::end);
	size = file.tellg();
	file.seekg(0, std::ios::beg);
	return size;
}

Bool InFile::readLine(String& line)
{
	if(std::getline(file, line)) return true;
	return false;
}

InFile::InFile(const Path& location)
{
	fileLocation = makeAbsolute(location);

	file = std::ifstream(location, std::ios::binary);
	if(file.is_open())
	{
		isOpen = true;
	}
	else
	{
		isOpen = false;
		logWarning("Could not open file at location (" + fileLocation.string() + ")!");
	}
}

OutFile::OutFile(const Path& location)
{
	fileLocation = makeAbsolute(location);

	const Path parentPath = fileLocation.parent_path();

	while(!doesPathExist(parentPath))
	{
		logDebug("The directory (" + parentPath.string() + ") does not exist and will be created!");
		createDirectory(parentPath);
	}

	file = std::ofstream(fileLocation, std::ios::trunc | std::ios::binary | std::ios::out);
	if(file.is_open())
	{
		isOpen = true;
	}
	else
	{
		isOpen = false;
		logError("Could not create file at location (" + fileLocation.string() + ")!");
	}
}

void InFile::read(char* data, const SizeT size)
{
	if(isOpen) file.read(data, size);
	else logError("The binary file" + fileLocation.string() + "could not be opened! Dont try to read it!");
}

void OutFile::write(const char* data, const SizeT size)
{
	if(isOpen) file.write(data, size);
	else logError("The binary file" + fileLocation.string() + "could not be opened!");
}

#pragma once

#include "Basics.hpp"
#include <fstream>

struct InFile
{
	Path fileLocation;
	Bool isOpen = false;
	std::ifstream file;

	~InFile();
	InFile(const Path& location);
	void read(char* data, SizeT size);

	template<typename T>
	void read(T& data)
	{
		read(static_cast<char*>(&data), sizeof(data));
	}

	Bool readLine(String& line)
	{
		if(std::getline(file, line)) return true;
		return false;
	}

	ULL fileSize()
	{
		ULL size = 0;
		file.seekg(0, std::ios::end);
		size = file.tellg();
		file.seekg(0, std::ios::beg);
		return size;
	}
};

struct OutFile
{
	Path fileLocation;
	Bool isOpen = false;
	std::ofstream file;

	~OutFile();
	OutFile(Path location);
	void write(const char* data, SizeT size);

	template<typename T>
	void write(T& data)
	{
		write(static_cast<char*>(&data), sizeof(data));
	}
};
#pragma once

#include "Basics.hpp"

using InputFileStream = std::ifstream;
using OutputFileStream = std::ofstream;

struct InFile
{
	InputFileStream file;
	Path            fileLocation;
	Bool            isOpen = false;

	~InFile();
	ULL fileSize();
	InFile() = delete;
	Bool readLine(String& line);
	InFile(const Path& location);
	void read(char* data, SizeT size);
	template <typename T> void read(T& data);
};

struct OutFile
{
	OutputFileStream file;
	Path             fileLocation;
	Bool             isOpen = false;

	~OutFile();
	OutFile() = delete;
	OutFile(const Path& location);
	void write(const char* data, SizeT size);
	template <typename T> void write(T& data);
};

template <typename T> void InFile::read(T& data)
{
	read(static_cast<char*>(&data), sizeof data);
}

template <typename T> void OutFile::write(T& data)
{
	write(static_cast<char*>(&data), sizeof data);
}

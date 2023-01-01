
#pragma once

#include "Basics.hpp"

struct InFile {
	Path fileLocation;
	Bool isOpen = false;
	std::ifstream file;

	~InFile();
	InFile(Path location);
	void read(char* data, SizeT size);

	template <typename T>
	void read(T& data) { read((char*)&data, sizeof(data));}
};

struct OutFile {
	Path fileLocation;
	Bool isOpen = false;
	std::ofstream file;

	~OutFile();
	OutFile(Path location);
	void write(char* data, SizeT size);

	template <typename T>
	void write(T& data) { write((char*)&data, sizeof(data)); }
};
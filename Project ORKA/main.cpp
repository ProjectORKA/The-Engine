//	Welcome fellow programmer!
//	My name is Samuel and im the creator of Project ORKA.
//	This is the open source code for the framework / game engine
//	Feel free to take a look at the code and point out all the errors
//	If you want to report bugs, request features or add your own code take a look at our GitHub:
//	https://github.com/ProjectORKA/Project-ORKA

//	Legal stuff:
//	This engine can NOT be used for your own personal or commercial use!
//	Do not distribute any source code outside of our official websites and platforms without permission!

//	The "Project ORKA" engine is divided into
//		1. data stored in structs (yes everything is public)
//		2. functions that act on that data in form of void or member functions
//	It has a minimal amount of inheritance and polymorphism and avoids de/constructors as much as possible 
//	This engine uses a left-handed coordinate system like Blender:
//	+X = right
//	+Y = forward
//	+Z = up

#include "WindowsEntry.hpp"

#include "Program.hpp"

int main(int argc, char* argv[]) {

	srand(static_cast <unsigned> (time(0)));

	if (true) //quickly switches between the game and custom testing code
	{
		Program program;		//<-- creates Program Data
		program.start();		//<-- starts the Program
	}
	else {
		//testing code
		/////////////////////////////////////////////////////////////////////////////////////////////////

		//std::ofstream outputFileStream;
		//outputFileStream.open("config.txt");		
		//outputFileStream.close();

//#include <stdio.h>
//
//		struct data {
//			Vector<Byte> memory;
//		};
//
//		Vector<ULL> data {};
//		for (int i = 0; i < count; i++) {
//			data.push_back(115564237);
//		}
//
//		std::ofstream myfile = std::ofstream("file.bin", std::ios::out | std::ios::binary);
//		myfile.write((char*)&data[0], sizeof(ULL) * data.size());
//		myfile.close();
//
//
//		std::ifstream myinfile = std::ifstream("file.bin", std::ios::in | std::ios::ate| std::ios::binary);
//
//		ULL data[] = new ULL(size);
//
//
//		myinfile.read((char*)p, sizeof(ULL) * count);
//		myinfile.close();
//
//		for (int i = 0; i < count; i++) {
//			std::cout << p[i] << "\n";
//		}

		/////////////////////////////////////////////////////////////////////////////////////////////////
		pause();
	}
	return 0;
}

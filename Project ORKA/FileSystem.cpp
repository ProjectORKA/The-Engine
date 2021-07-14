
#include "FileSystem.hpp"

void loadString(String& string, Path path) {
	std::ifstream stream(path, std::ios::in);
	if(!stream.is_open()) logError("String Cant be read!");
	std::stringstream sstr;
	sstr << stream.rdbuf();
	string = sstr.str();
	stream.close();
}

#include "FileSystem.hpp"

String loadString(Path path) {
	String s;
	std::ifstream stream(path, std::ios::in);
	if(!stream.is_open()) logError(String("String Cant be read! (").append(path.string()).append(")"));
	std::stringstream sstr;
	sstr << stream.rdbuf();
	s = sstr.str();
	stream.close();
	return s;
}
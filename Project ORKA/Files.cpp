
#include "Files.hpp"

void loadString(String& string, Path path) {
	std::ifstream stream(path, std::ios::in);
	assert(stream.is_open());
	std::stringstream sstr;
	sstr << stream.rdbuf();
	string = sstr.str();
	stream.close();
}
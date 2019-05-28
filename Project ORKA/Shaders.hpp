#pragma once
#ifndef SHADERS_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#define GLEW_STATIC
#include "GL/glew.h"


class Shader {
public:
	bool loaded = false;
	GLuint ProgramID;

	Shader();
	~Shader();
	void init(const char * vertex_file_path, const char * fragment_file_path);
	void load(const char * vertex_file_path, const char * fragment_file_path);
	void use();
};



#endif // !SHADERS_HPP

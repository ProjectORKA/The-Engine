#pragma once
#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <map>
#define GLEW_STATIC
#include "GL/glew.h"
#include "glm/glm.hpp"

class Mesh {
public:
	bool loaded;
	GLuint vertexBuffer;
	Mesh();
	void load();
	void render();
	~Mesh();
};

class MeshHandler {
public:
	std::map<std::string, Mesh> meshMap;
	MeshHandler();
};
#endif // !MESH_HPP
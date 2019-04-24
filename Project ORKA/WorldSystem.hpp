#pragma once
#ifndef WORLDSYSTEM_HPP
#define WORLDSYSTEM_HPP

#include <vector>
#include "glm/glm.hpp"
#include "Sky.hpp"
#include "Entity.hpp"

class Chunk {
public:
	std::vector<Entity *> entities;
	Chunk();
	~Chunk();
};

class WorldSystem {
public:
	Sky sky;
	Chunk chunk; //[TODO] turn into octree structure
};
#endif // !WORLDSYSTEM_HPP
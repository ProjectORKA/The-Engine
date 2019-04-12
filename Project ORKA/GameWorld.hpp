#pragma once

#include <vector>
#include "Sky.hpp"

class Triangle {
public:
	std::vector<glm::vec2> vertecies;
	
	Triangle(){
		vertecies.push_back(glm::vec2(-1.0f, -1.0f));
		vertecies.push_back(glm::vec2(-1.0f, +1.0f));
		vertecies.push_back(glm::vec2(+1.0f, +0.0f));
	};
};

class GameWorld {
public:
	Sky sky;
	Triangle triangle;
};


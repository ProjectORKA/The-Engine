#pragma once
#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <vector>

class Entity {
public:
	std::string type;
	Entity();
};

class Triangle : public Entity {
public:
	Triangle();
};
#endif // !ENTITY_HPP
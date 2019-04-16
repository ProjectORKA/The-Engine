#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "Sky.hpp"

class Component {
public:
	std::string componentType = "empty";
};

class MeshComponent : public Component {
public:
	std::string path = "";
	MeshComponent(std::string path) : path(path) {
		componentType = "mesh";
	}
};

class Entity {
public:
	std::vector<Component *> components;
	~Entity() {
		for (int i = 0; i < components.size(); i++) {
			delete components[i];
		}
		components.clear();
	}
};

class Triangle : public Entity {
public:
	Triangle() {
		Component * tmp = new MeshComponent("objects/triangle");
		components.push_back(tmp);
	}
};

class GameWorld {
public:
	Sky sky;
	std::vector<Entity *> entities;
	GameWorld() {
		Entity * tmp = new Triangle;
		entities.push_back(tmp);
	}
};
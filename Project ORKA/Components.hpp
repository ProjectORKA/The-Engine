#pragma once
#ifndef COMPUNENTS_HPP
#define COMPONENTS_HPP

#include <string>

class Component {
public:
	std::string componentType = "empty";
};

class MeshComponent : public Component {
public:
	std::string path = "empty";
	MeshComponent(std::string path);
};
#endif // !COMPUNENTS_HPP
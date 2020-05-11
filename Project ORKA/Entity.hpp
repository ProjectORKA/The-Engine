#pragma once

#include "Basics.hpp"

struct Entity {
	virtual void process() {
		std::cout << "i am an entity" << std::endl;
	}
};

struct Monkey : virtual Entity{

	void process() {
		std::cout << "i am a monkey" << std::endl;
	}
};
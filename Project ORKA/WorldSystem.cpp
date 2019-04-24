
#include "WorldSystem.hpp"

Chunk::Chunk() {
	Entity * tmp = new Triangle;
	entities.push_back(tmp);
}
Chunk::~Chunk() {
	for (int i = 0; i < entities.size(); i++) {
		delete entities[i];
	}
	entities.clear();
}

#include "Program.hpp"

Chunk::Chunk()
{
	int gridSize = 10;
	for (int x = 0; x < gridSize; x++) {
		for (int y = 0; y < gridSize; y++) {
			addTriangle(entityComponentSystem);
			entityComponentSystem.transformationSystem.transformations.back().location.x = 3 * (x - gridSize / 2);
			entityComponentSystem.transformationSystem.transformations.back().location.y = 3 * (y - gridSize / 2);
		}
	}
	debugPrint("|--Chunk was created!");
}
Chunk::~Chunk()
{
	debugPrint("|--Chunk was destroyed!");
}
Sky::Sky() {
	skyColor = glm::vec3(0.0f);
}
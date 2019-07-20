
#include "Program.hpp"

Chunk::Chunk()
{
	int gridSize = 201;
	for (int x = 0; x < gridSize; x++) {
		for (int y = 0; y < gridSize; y++) {

			static int flopper = 0;

			spawnEntity(entityComponentSystem, entityComponentSystem.entityTypes.names[flopper]);

			if (flopper != 0) {
				entityComponentSystem.transformationSystem.transformations.back().location.x = 3.0f * (x - gridSize / 2.0f);
				entityComponentSystem.transformationSystem.transformations.back().location.y = 3.0f * (y - gridSize / 2.0f);
			}

			flopper++;

			if (flopper >= entityComponentSystem.entityTypes.numberOfEntityTypes) {
				flopper = 0;
			}
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
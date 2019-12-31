
#include "Program.hpp"

void addRenderingComponent(Entity& entity, GameSimulation& gameSimulation, const char * name) {
	for (int i = 0; i < gameSimulation.renderComponentSystem.names.size(); i++) {
		if (name == gameSimulation.renderComponentSystem.names[i]) {
			entity.indices[RenderComponentType] = i;
			return;
		}
	}
	gameSimulation.renderComponentSystem.names.push_back(name);
	entity.indices[RenderComponentType] = gameSimulation.renderComponentSystem.names.size() - 1;
}

void addModelMatrix(Entity& entity, ModelMatrixSystem& modelMatrixSystem)
{
	modelMatrixSystem.mutex.lock();
	modelMatrixSystem.modelMatrices.emplace_back();
	modelMatrixSystem.mutex.unlock();
	entity.indices[ModelMatrixComponentType] = modelMatrixSystem.modelMatrices.size() -1;
}

void addModelMatrix(Entity& entity, ModelMatrixSystem & modelMatrixSystem, Transformation & transformation)
{
	modelMatrixSystem.mutex.lock();
	modelMatrixSystem.modelMatrices.push_back(transformation.calculateModelMatrix());
	modelMatrixSystem.mutex.unlock();
	entity.indices[ModelMatrixComponentType] = modelMatrixSystem.modelMatrices.size() - 1;
}

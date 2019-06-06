
#include "Program.hpp"

void addEmptyEntity(ECS & entityComponentSystem) {
	entityComponentSystem.entities.emplace_back();
	entityComponentSystem.transformationSystem.transformations.emplace_back();
	entityComponentSystem.transformationSystem.interfaceLayer.push_back(&entityComponentSystem.transformationSystem.transformations.back());
}

void addTriangle(ECS & entityComponentSystem)
{
	addEmptyEntity(entityComponentSystem);
	entityComponentSystem.entities.back()[0] = 1;	// 1 means "has transformation component"
}


glm::mat4 Transformation::modelMatrix()
{
	//make more performant
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::scale(modelMatrix, scale);
	modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0, 1, 0));
	modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3(1, 0, 0));
	modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3(0, 0, 1));
	modelMatrix = glm::translate(modelMatrix, location);
	return modelMatrix;

}

#include "Program.hpp"

glm::vec3 Transformation::getLocation()
{
	return location;
}

float Transformation::getScale()
{
	return scale;
}

glm::mat4 Transformation::calculateModelMatrix()
{
	return translationMatrix * scaleMatrix * rotationMatrix;
}

void Transformation::setScale(float scale)
{
	this->scale = scale;
	this->scaleMatrix = glm::scale(glm::mat4(1), glm::vec3(scale));
}

void Transformation::addRotation(glm::vec3 rotation)
{
	this->rotation += rotation;
	this->rotationMatrix = glm::rotate(glm::rotate(glm::rotate(glm::mat4(1), this->rotation.y, glm::vec3(0, 1, 0)), this->rotation.x, glm::vec3(1, 0, 0)), this->rotation.z, glm::vec3(0, 0, 1));
}

void Transformation::setRotation(glm::vec3 rotation)
{
	this->rotation = rotation;
	this->rotationMatrix = glm::rotate(glm::rotate(glm::rotate(glm::mat4(1), this->rotation.y, glm::vec3(0, 1, 0)), this->rotation.x, glm::vec3(1, 0, 0)), this->rotation.z, glm::vec3(0, 0, 1));
}

void Transformation::addTranslation(glm::vec3 location)
{
	this->location += location;
	this->translationMatrix = glm::translate(glm::mat4(1), this->location);
}

void Transformation::setTranslation(glm::vec3 location)
{
	this->location = location;
	this->translationMatrix = glm::translate(glm::mat4(1), this->location);
}

void addTransformation(Entity& entity, GameSimulation& gameSimulation) {
	gameSimulation.transformationSystem.mutex.lock();
	///
	gameSimulation.transformationSystem.transformations.emplace_back();

	entity.indices[TransformationComponentType] = gameSimulation.transformationSystem.transformations.size() - 1;
	///
	gameSimulation.transformationSystem.mutex.unlock();
}

void addTransformation(Entity& entity, GameSimulation& gameSimulation, Transformation & transformation) {
	gameSimulation.transformationSystem.mutex.lock();
	///
	gameSimulation.transformationSystem.transformations.push_back(transformation);
	entity.indices[TransformationComponentType] = gameSimulation.transformationSystem.transformations.size() - 1;
	///
	gameSimulation.transformationSystem.mutex.unlock();
}
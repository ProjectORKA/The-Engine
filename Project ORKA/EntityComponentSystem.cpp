
#include "Program.hpp"

//entity
void addEmptyEntity(ECS & entityComponentSystem) {
	entityComponentSystem.entities.emplace_back();
	entityComponentSystem.names.emplace_back();
}


//camera
void addCamera(CameraSystem & cameraSystem)
{

	cameraSystem.cameraLocation.push_back(glm::vec3(0.0f));
	cameraSystem.cameraRotationX.push_back(0.0f);
	cameraSystem.cameraRotationZ.push_back(0.0f);
	cameraSystem.cameraSpeed.push_back(10.0f);

	cameraSystem.forwardVector.push_back(glm::vec3(0, 1, 0));
	cameraSystem.rightVector.push_back(  glm::vec3(1, 0, 0));
	cameraSystem.upVector.push_back(     glm::vec3(0, 0, 1));

	cameraSystem.viewMatrices.push_back(glm::lookAt( glm::vec3(0), glm::vec3(0,1,0), glm::vec3(0,0,1)));
}
void rotateCamera(CameraSystem & cameraSystem, unsigned int cameraID, float x, float y) {
	cameraSystem.cameraRotationX[cameraID] -= 0.005 * y;
	cameraSystem.cameraRotationZ[cameraID] += 0.005 * x;
}
void pocessCameras(CameraSystem & cameraSystem)
{
	for (int i = 0; i < cameraSystem.cameraLocation.size(); i++) {
		
		//prevent looking upside down
		if (cameraSystem.cameraRotationX[i] < -1) cameraSystem.cameraRotationX[i] = -1;
		if (cameraSystem.cameraRotationX[i] > +1) cameraSystem.cameraRotationX[i] = +1;

		//calculate directional vectors
		cameraSystem.forwardVector[i] = glm::vec3(
			cos(cameraSystem.cameraRotationX[i]) * sin(cameraSystem.cameraRotationZ[i]),
			cos(cameraSystem.cameraRotationX[i]) * cos(cameraSystem.cameraRotationZ[i]),
			sin(cameraSystem.cameraRotationX[i])
		);

		cameraSystem.rightVector[i] = glm::vec3(
			-sin(cameraSystem.cameraRotationZ[i] - PI/2),
			-cos(cameraSystem.cameraRotationZ[i] - PI/2),
			0
		);
		cameraSystem.upVector[i] = glm::cross(cameraSystem.rightVector[i], cameraSystem.forwardVector[i]);

		//calculate viewMatrix
		cameraSystem.viewMatrices[i] = glm::lookAt(cameraSystem.cameraLocation[i], cameraSystem.cameraLocation[i] + cameraSystem.forwardVector[i], cameraSystem.upVector[i]);
	}
}
CameraSystem::CameraSystem()
{
	addCamera(*this);
}

//transformation
void addTriangle(ECS & ecs)
{
	addEmptyEntity(ecs);
	ecs.names.back() = "triangle";
	addTransformationComponent(ecs, Transformation());
}
void addTransformationComponent(ECS & entityComponentSystem, Transformation transformation) {
	
	entityComponentSystem.transformationSystem.transformations.push_back(transformation);
	entityComponentSystem.transformationSystem.interfaceLayer.push_back(&entityComponentSystem.transformationSystem.transformations.back());
	entityComponentSystem.entities.back()[0] = 1;

}
glm::mat4 Transformation::modelMatrix()
{
	//make more performant
	return glm::scale(glm::rotate(glm::rotate(glm::rotate(glm::translate(glm::mat4(1), location), rotation.z, glm::vec3(0, 0, 1)), rotation.x, glm::vec3(1, 0, 0)), rotation.y, glm::vec3(0, 1, 0)), scale);
}

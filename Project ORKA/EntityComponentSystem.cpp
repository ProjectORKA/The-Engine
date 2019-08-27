
#include "Program.hpp"

//camera
void rotateCamera(Camera & camera, float x, float y) {
	camera.cameraRotationX -= camera.mouseSensitivity * y;
	camera.cameraRotationZ += camera.mouseSensitivity * x;
}
void pocessCamera(Camera & camera)
{
	//prevent looking upside down

	float cap = PI / 2;

	if (camera.cameraRotationX < -cap) {
		camera.cameraRotationX = -cap;
	}
	if (camera.cameraRotationX > +cap) {
		camera.cameraRotationX = +cap;
	}

	//calculate directional vectors
	camera.forwardVector = glm::vec3(
		cos(camera.cameraRotationX) * sin(camera.cameraRotationZ),
		cos(camera.cameraRotationX) * cos(camera.cameraRotationZ),
		sin(camera.cameraRotationX)
	);

	camera.rightVector = glm::vec3(
		-sin(camera.cameraRotationZ - PI / 2),
		-cos(camera.cameraRotationZ - PI / 2),
		0
	);
	camera.upVector = glm::cross(camera.rightVector, camera.forwardVector);

	//calculate viewMatrix
	camera.viewMatrix = glm::lookAt(camera.cameraLocation, camera.cameraLocation + camera.forwardVector, camera.upVector);
}

void addEntityType(EntityTypes & entityTypes, std::string name, ComponentStructure structure) {

	bool nameExists = false;
	unsigned int nameExistsAtLocation = 0;
	for (unsigned int i = 0; i < entityTypes.numberOfEntityTypes; i++) {
		if (entityTypes.names[i] == name) {
			nameExists = true;
			nameExistsAtLocation = i;
		}
	}

	if (nameExists) {
		if (entityTypes.structure[nameExistsAtLocation] == structure) {
			debugPrint("EntityType does already exist!");
			return;
		}
		else {
			debugPrint("Name for EntityType is already in use!");
			return;
		}
	}
	else {
		entityTypes.numberOfEntityTypes++;
		entityTypes.names.push_back(name);
		entityTypes.structure.push_back(structure);
		std::vector<Entity> tmp;
		entityTypes.entityArrays.push_back(tmp);
	}
};

void spawnEntity(EntityComponentSystem & ecs, std::string name)
{
	//name to index
	unsigned int entityTypeIndex = UINT_MAX;
	for (unsigned int i = 0; i < ecs.entityTypes.numberOfEntityTypes; i++) {
		if (ecs.entityTypes.names[i] == name) {
			entityTypeIndex = i;
		}
	}
	if (entityTypeIndex == UINT_MAX) {
		debugPrint("Entity type doesn't exist!");
	}
	else {
		//create the data
		Entity tmp;
		//transformation component
		if (ecs.entityTypes.structure[entityTypeIndex][0]) {
			addTransformationComponent(ecs.transformationSystem, tmp);
		}

		/////////////////////////////////////////////////////////////////////////////
		//		//another component												   //
		//	if (ecs.entityTypes.structure[entityTypeIndex][999]) {				   //
		//		tmp.componentIndices.push_back(ecs.anotherSystem.anothers.size()); //
		//		addAnotherComponent(ecs);										   //
		//	}																	   //
		/////////////////////////////////////////////////////////////////////////////

		//spawn the entity
		ecs.entityTypes.entityArrays[entityTypeIndex].push_back(tmp);
	}
}

//transformation
void addTransformationComponent(TransformationSystem & transformationSystem, Entity & entity) {
	entity.componentIndices.push_back(transformationSystem.transformations.size());
	transformationSystem.transformations.push_back(Transformation());
}

void calculateModelMatrix(glm::mat4 & matrix, Transformation & transformation)
{
	matrix = glm::scale(glm::rotate(glm::rotate(glm::rotate(glm::translate(glm::mat4(1), transformation.location), transformation.rotation.z, glm::vec3(0, 0, 1)), transformation.rotation.x, glm::vec3(1, 0, 0)), transformation.rotation.y, glm::vec3(0, 1, 0)), transformation.scale);
}

//mesh
//void addMeshComponent(ECS & entityComponentSystem, const char * path){
//	entityComponentSystem.entitySystem.usedComponents.back()[1] = true;
//	entityComponentSystem.entitySystem.meshIndex.back() = entityComponentSystem.
//
//	entityComponentSystem.transformationSystem.transformations.push_back(transformation);
//}


EntityTypes::EntityTypes()
{
	numberOfEntityTypes = 0;


	////empty
	//ComponentStructure emptyStructure;
	//addEntityType(*this, "empty", emptyStructure);

	//error
	ComponentStructure errorStructure;
	errorStructure[0] = true;
	addEntityType(*this, "error", errorStructure);

	//monkey
	ComponentStructure monkeyStructure;
	monkeyStructure[0] = true;
	addEntityType(*this, "monkey", monkeyStructure);

	//cube
	ComponentStructure cubeStructure;
	cubeStructure[0] = true;
	addEntityType(*this, "cube", cubeStructure);

	//icosphere
	ComponentStructure icosphereStructure;
	icosphereStructure[0] = true;
	addEntityType(*this, "icosphere", icosphereStructure);

	//triangle
	ComponentStructure triangleStructure;
	triangleStructure[0] = true;
	addEntityType(*this, "triangle", triangleStructure);

	//plane
	ComponentStructure planeStructure;
	planeStructure[0] = true;
	addEntityType(*this, "plane", planeStructure);

	////point
	//ComponentStructure pointStructure;
	//pointStructure[0] = true;
	//addEntityType(*this, "point", pointStructure);
	//
	////point cloud
	//ComponentStructure pointCloudStructure;
	//pointCloudStructure[0] = true;
	//addEntityType(*this, "point cloud", pointCloudStructure);

	//tree
	ComponentStructure treeStructure;
	treeStructure[0] = true;
	addEntityType(*this, "tree", treeStructure);

	//terrain mesh
	ComponentStructure terrainStructure;
	terrainStructure[0] = true;
	addEntityType(*this, "terrain", terrainStructure);
}
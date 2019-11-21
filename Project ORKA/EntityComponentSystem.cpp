
#include "Program.hpp"


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
		if (entityTypes.structures[nameExistsAtLocation] == structure) {
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
		entityTypes.structures.push_back(structure);
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
		if (ecs.entityTypes.structures[entityTypeIndex][TRANSFORMATION]) {
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

void addComponent(ComponentStructure structure, unsigned int componentID)
{
	structure[componentID] = true;
}

EntityTypes::EntityTypes()
{
	numberOfEntityTypes = 0; //[TODO] check if necessary

	//error
	ComponentStructure player;
	addComponent(player,TRANSFORMATION);
	addEntityType(*this, "player", player);

	//error
	ComponentStructure errorStructure;
	addComponent(errorStructure, TRANSFORMATION);
	addEntityType(*this, "error", errorStructure);

	//monkey
	ComponentStructure monkeyStructure;
	addComponent(monkeyStructure, TRANSFORMATION);
	addEntityType(*this, "monkey", monkeyStructure);

	//cube
	ComponentStructure cubeStructure;
	addComponent(cubeStructure, TRANSFORMATION);
	addEntityType(*this, "cube", cubeStructure);

	//icosphere
	ComponentStructure icosphereStructure;
	addComponent(icosphereStructure, TRANSFORMATION);
	addEntityType(*this, "icosphere", icosphereStructure);

	//triangle
	ComponentStructure triangleStructure;
	addComponent(triangleStructure, TRANSFORMATION);
	addEntityType(*this, "triangle", triangleStructure);

	//plane
	ComponentStructure planeStructure;
	addComponent(planeStructure, TRANSFORMATION);
	addEntityType(*this, "plane", planeStructure);

	////point
	//ComponentStructure pointStructure;
	//pointStructure.addComponent(TRANSFORMATION);
	//addEntityType(*this, "point", pointStructure);
	//
	////point cloud
	//ComponentStructure pointCloudStructure;
	//pointCloudStructure.addComponent(TRANSFORMATION);
	//addEntityType(*this, "point cloud", pointCloudStructure);

	//tree
	ComponentStructure treeStructure;
	addComponent(treeStructure, TRANSFORMATION);
	addEntityType(*this, "tree", treeStructure);

	//terrain mesh
	ComponentStructure terrainStructure;
	addComponent(terrainStructure, TRANSFORMATION);
	addEntityType(*this, "terrain", terrainStructure);
}


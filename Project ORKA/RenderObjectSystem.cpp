
#include "RenderObjectSystem.hpp"

void RenderObjectSystem::create() {
	meshSystem.create();
	textureSystem.create();
	shaderSystem.create();


	addRenderObject("spaceShip", "spaceShip", "spaceShip", "spaceShip");
	addRenderObject("spaceShipLOD", "spaceShipLOD", "spaceShip", "spaceShip");
	addRenderObject("earth", "earth", "earth", "primitive");

	addRenderObject("wireBox", "wireMeshBox", "default", "debug");
	addRenderObject("default", "default", "default", "primitive");
	addRenderObject("plane", "plane", "default", "debug");
	addRenderObject("terrain", "terrain", "default", "primitive");
	addRenderObject("boundingBox", "boundingBox", "default", "debug");
	addRenderObject("skeleton", "skeleton", "skeleton", "primitive");
	addRenderObject("monkey", "monkey", "default", "debug");
	addRenderObject("sky", "sky", "stars", "primitive");
}
void RenderObjectSystem::addRenderObject(String name, Name meshName, Name textureName, Name shaderName) {
	
	//find the specified mesh
	auto itM = meshSystem.meshNames.find(meshName);
	if (itM == meshSystem.meshNames.end()) {
		//maybe its not loaded
		meshSystem.loadMesh(meshName);
		itM = meshSystem.meshNames.find(meshName);
	}

	auto itS = shaderSystem.shaderNames.find(shaderName);
	auto itT = textureSystem.textureNames.find(textureName);

	if (itM != meshSystem.meshNames.end() && itS != shaderSystem.shaderNames.end() && itT != textureSystem.textureNames.end()) {
		renderObjects.emplace_back();
		renderObjects.back().meshID = itM->second;
		renderObjects.back().shaderID = itS->second;
		renderObjects.back().textureID = itT->second;
		nameToIndex[name] = renderObjects.size() - 1;
	}
	else {
		logDebug(String("Could not find assets to create Render Object. (").append(name).append(" = ").append(meshName.data).append("|").append(textureName.data).append("|").append(shaderName.data).append(")"));
	}
}
void RenderObjectSystem::render(String name) {
	auto it = nameToIndex.find(name);
	if (it != nameToIndex.end()) {
		RenderObject& renderObject = renderObjects[it->second];
		shaderSystem.useShader(renderObject.shaderID);
		textureSystem.render(renderObject.textureID);
		meshSystem.renderMesh(renderObject.meshID);
	}
	else {
		logDebug("RenderObject With this name doesent exist.");
	}
}

void RenderObjectSystem::destroy()
{
	meshSystem.destroy();
	shaderSystem.destroy();
	textureSystem.destroy();

	nameToIndex.clear();
	renderObjects.clear();
}


#include "RenderObjectSystem.hpp"

void RenderObjectSystem::create() {
	meshSystem.create();
	textureSystem.create();
	shaderSystem.create();

	//space scene
	addRenderObject("spaceShip", "spaceShip", "spaceShip", "primitive");
	addRenderObject("spaceShipLOD", "spaceShipLOD", "spaceShip", "primitive");
	addRenderObject("earth", "earth", "earth", "primitive");

	//general objects
	addRenderObject("sky", "sky", "stars", "sky");
	addRenderObject("monkey", "monkey", "uv", "primitive");
	addRenderObject("plane", "plane", "default", "primitive");
	addRenderObject("default", "default", "default", "primitive");
	addRenderObject("skeleton", "skeleton", "skeleton", "primitive");
	addRenderObject("wireBox", "wireMeshBox", "default", "primitive");

	//planet scene
	addRenderObject("monkey0", "monkey0", "uv", "primitive");
	addRenderObject("monkey1", "monkey1", "uv", "primitive");
	addRenderObject("monkey2", "monkey2", "uv", "primitive");
	addRenderObject("monkey3", "monkey3", "uv", "primitive");
	addRenderObject("monkey4", "monkey4", "uv", "primitive");
	addRenderObject("monkey5", "monkey5", "uv", "primitive");
	addRenderObject("monkey6", "monkey6", "uv", "primitive");
	addRenderObject("monkey7", "monkey7", "uv", "primitive");
	addRenderObject("terrain", "terrain", "default2", "primitive");
	addRenderObject("boundingBox", "boundingBox", "default", "primitive");
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
		logError(String("Could not find assets to create Render Object. (").append(name).append(" = ").append(meshName.data).append("|").append(textureName.data).append("|").append(shaderName.data).append(")"));
	}
}
void RenderObjectSystem::render(String name) {
	auto it = nameToIndex.find(name);
	if (it != nameToIndex.end()) {
		RenderObject& renderObject = renderObjects[it->second];
		shaderSystem.useShader(renderObject.shaderID);
		textureSystem.use(renderObject.textureID);
		meshSystem.renderMesh(renderObject.meshID);
	}
	else {
		logDebug(String("RenderObject With this name doesent exist. (").append(name).append(")"));
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
